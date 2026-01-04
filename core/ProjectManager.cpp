#include "ProjectManager.h"
#include "Common.h"
#include "nfd.h"
#include <fstream>
#include <iostream>

namespace Roar {

void ProjectManagerLayer::OnEvent(Event &event) {}

void ProjectManagerLayer::OnUpdate(float ts) {}

void ProjectManagerLayer::OnRender() {}

bool ProjectManagerLayer::OpenProject() {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_PickFolder(NULL, &outPath);

    if (result == NFD_OKAY) {
        RO_LOG_INFO("Success!");
        RO_LOG_INFO(outPath);
        projectRoot = outPath;
        scenePath = projectRoot / "Scenes" / "Game.ro";
        gameLibPath = projectRoot / "Game.dll";
        free(outPath);
    } else if (result == NFD_CANCEL) {
        puts("User pressed cancel.");
    } else {
        RO_LOG_ERR(NFD_GetError());
    }

    if (result == NFD_OKAY)
        return true;
    return false;
}

void ProjectManagerLayer::BuildProject() {
    // 1. Define the command
    // Note: 'dotnet' is usually in the global PATH on both Windows and Linux.
    // We use forward slashes '/' because dotnet accepts them on Windows too.
    fs::path projectDir = projectRoot;
    std::string projectName = projectDir.filename().string();
    fs::path projectPath = projectDir / projectName;
    RO_LOG_INFO("Project Name: {}", projectName);
    std::string buildCmd = "dotnet build " + projectPath.string() + ".csproj -c Release ";

    // 2. Run in a background thread to keep UI responsive
    std::thread buildThread([buildCmd]() {
        std::cout << "Starting Build..." << std::endl;
        RO_LOG_INFO("Starting Build...");

        auto result = ProcessRunner::Execute(buildCmd);

        if (result.ExitCode == 0) {
            std::cout << "Build Success!" << std::endl;
            RO_LOG_INFO("Build Success !");
            // TODO: Queue a "Reload Assembly" task on the main thread
        } else {
            RO_LOG_ERR("Build Failed: {}", result.Output);
        }
    });

    // Detach so it runs independently, or join if you want to wait.
    buildThread.detach();
}

void CopyDirectoryRecursively(const fs::path &source, const fs::path &destination) {
    try {
        // Check if source exists
        if (!fs::exists(source)) {
            std::cerr << "Error: Source directory does not exist: " << source << std::endl;
            return;
        }

        // Create the destination folder if it doesn't exist
        //    (copy() would fail if the parent folder didn't exist)
        if (!fs::exists(destination)) {
            fs::create_directories(destination);
        }

        // Copy recursively
        //    overwrite_existing: Update files if we are re-building
        //    recursive: Copy subfolders (lib/mono/4.5/...)
        fs::copy(source, destination, fs::copy_options::recursive | fs::copy_options::overwrite_existing);

        std::cout << "Copied: " << source.filename() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "File System Error: " << e.what() << std::endl;
    }
}

void ProjectManagerLayer::ExportProject() {
    RO_LOG_INFO("Starting Export process");

    // COMPILE THE GAME (Release Mode)
    // We assume the project file is named the same as the folder for this example
    fs::path projectDir = projectRoot;
    std::string projectName = projectDir.filename().string();
    RO_LOG_INFO("Project Name: {}", projectName);
    fs::path csprojFile = projectDir / (projectName + ".csproj");

    std::string buildCmd = "dotnet build " + csprojFile.string() + " -c Release";

    auto result = ProcessRunner::Execute(buildCmd);
    if (result.ExitCode != 0) {
        RO_LOG_ERR("Export failed during compilation: {}", result.Output);
        return;
    }
    std::cout << "Compilation Successful." << std::endl;

    // PREPARE DESTINATION FOLDER
    fs::path exportPath = projectRoot / (projectName + "_Release");
    fs::path gameDataPath = exportPath / "GameData";
    fs::path monoPath = gameDataPath / "mono";

    try {
        // Clean old build if exists
        if (fs::exists(exportPath))
            fs::remove_all(exportPath);

        fs::create_directories(exportPath);
        fs::create_directories(gameDataPath);
        fs::create_directories(monoPath);
    } catch (std::exception &e) {
        std::cerr << "File System Error: " << e.what() << std::endl;
        RO_LOG_ERR("Fil system error: {}", e.what());
        return;
    }

// COPY THE ENGINE RUNTIME (The "Player")
// Detect OS extension
#ifdef _WIN32
    std::string exeExt = ".exe";
    std::string libExt = ".dll";
#else
    std::string exeExt = "";    // Linux binaries have no extension
    std::string libExt = ".so"; // Shared objects
#endif

    // We assume you have a "Player.exe" in your engine's bin folder

    fs::path sourcePlayer = fs::current_path() / ("RoarEngine" + exeExt);
    fs::path destPlayer = exportPath / (projectName + exeExt); // Rename to GameName.exe

    RO_LOG_INFO("Copying engine runtime. Copied {} to {}", sourcePlayer.string(), destPlayer.string());

    // Copy the Executable
    fs::copy_file(sourcePlayer, destPlayer, fs::copy_options::overwrite_existing);

    // Copy the Mono Runtime Bridge (mono-2.0-sgen.dll / .so)
    // Adjust naming based on what your engine specifically links against
    fs::copy_file(fs::current_path() / "mono-2.0-sgen.dll", exportPath / "mono-2.0-sgen.dll", fs::copy_options::skip_existing);

    // COPY GAME BINARIES
    // We copy the DLLs into "GameData" to keep the root clean
    fs::path compiledDll = projectDir / "Build" / "bin" / (projectName + ".dll");
    fs::path engineCoreDll = fs::current_path() / "RoarScriptCore.dll";


    fs::copy_file(compiledDll, gameDataPath / (projectName + ".dll"), fs::copy_options::overwrite_existing);
    fs::copy_file(engineCoreDll, gameDataPath / "RoarScriptCore.dll", fs::copy_options::overwrite_existing);

    // COPY MONO STUFF
    CopyDirectoryRecursively(fs::current_path() / "mono" / "lib", monoPath / "lib");
    CopyDirectoryRecursively(fs::current_path() / "mono" / "etc", monoPath / "etc");

    // GENERATE BOOT CONFIG
    // This tells Player.exe what to load
    std::ofstream bootFile(gameDataPath / "boot.config");
    bootFile << "entry_assembly=" << projectName << ".dll" << std::endl;
    bootFile.close();

    RO_LOG_INFO("Export Compelete! Output: {}", exportPath.string());
}

void CreateGameProject(const std::string &projectRoot, const std::string &projectName, const std::string &engineDllPath) {
    fs::path rootPath = projectRoot;
    fs::path sourcePath = rootPath / "Source";
    fs::path buildPath = rootPath / "Build";
    fs::path scenePath = rootPath / "Scenes";
    fs::path csprojPath = rootPath / (projectName + ".csproj");

    try {
        // fs::create_directories(rootPath);
        fs::create_directories(sourcePath);
        fs::create_directories(buildPath);
        fs::create_directories(scenePath);
        std::ofstream ofs(scenePath / "Game.ro");
        ofs.close();
    } catch (std::exception &e) {
        std::cerr << "Failed to create directories: " << e.what() << std::endl;
        return;
    }

    // We use a raw string literal R"(...)" for easy multi-line XML
    std::string xmlContent = R"(<Project>
  <PropertyGroup>
    <BaseIntermediateOutputPath>Build\obj\</BaseIntermediateOutputPath>
  </PropertyGroup>

  <Import Project="Sdk.props" Sdk="Microsoft.NET.Sdk" />

  <PropertyGroup>
    <TargetFramework>net472</TargetFramework>
    <ImplicitUsings>disable</ImplicitUsings>
    <Nullable>disable</Nullable>

    <OutputPath>Build\bin\</OutputPath>
    <AppendTargetFrameworkToOutputPath>false</AppendTargetFrameworkToOutputPath>
    
    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>
  </PropertyGroup>

  <ItemGroup>
    <Reference Include="RoarScriptCore">
      <HintPath>)" + engineDllPath +
                             R"(</HintPath>
    </Reference>
  </ItemGroup>

  <ItemGroup>
    <Compile Include="Source\**\*.cs" />
  </ItemGroup>

  <Import Project="Sdk.targets" Sdk="Microsoft.NET.Sdk" />
</Project>)";

    std::ofstream file(csprojPath);
    if (file.is_open()) {
        file << xmlContent;
        file.close();
        std::cout << "Successfully created project: " << csprojPath << std::endl;
    } else {
        std::cerr << "Failed to write .csproj file!" << std::endl;
    }

    // Create a dummy script so the folder isn't empty
    std::ofstream scriptFile(sourcePath / "Main.cs");
    scriptFile << "using System;\nusing RoarEngine;\n\nnamespace Game {\n\tpublic class Main {\n\t}\n}";
    scriptFile.close();
}

bool ProjectManagerLayer::CreateProject() {
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_PickFolder(NULL, &outPath);

    if (result == NFD_OKAY) {
        RO_LOG_INFO("Success!");
        RO_LOG_INFO(outPath);
        projectRoot = outPath;
        scenePath = projectRoot / "Scenes" / "Game.ro";
        gameLibPath = projectRoot / "Game.dll";
        free(outPath);
    } else if (result == NFD_CANCEL) {
        puts("User pressed cancel.");
    } else {
        RO_LOG_ERR(NFD_GetError());
    }

    std::string projectName = projectRoot.filename().string();
    CreateGameProject(projectRoot.string(), projectName, std::filesystem::current_path().string());

    if (result == NFD_OKAY)
        return true;
    return false;
}

} // namespace Roar
