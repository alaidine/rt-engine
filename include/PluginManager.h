#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
typedef HMODULE LibraryHandle;
#else
#include <dlfcn.h>
typedef void *LibraryHandle;
#endif

class LibraryLoader {
  public:
    static LibraryHandle Load(const std::string &path) {
#if defined(_WIN32)
        // Windows locks the file when loaded. For hot-reloading,
        // you usually copy 'plugin.dll' to 'plugin_temp.dll' here
        // before loading!
        LibraryHandle handle = LoadLibraryA(path.c_str());
        if (!handle)
            std::cerr << "Win32 Load Error: " << GetLastError() << std::endl;
        return handle;
#else
        // RTLD_NOW loads all symbols immediately
        LibraryHandle handle = dlopen(path.c_str(), RTLD_NOW);
        if (!handle)
            std::cerr << "Linux Load Error: " << dlerror() << std::endl;
        return handle;
#endif
    }

    static void Unload(LibraryHandle handle) {
        if (!handle)
            return;
#if defined(_WIN32)
        FreeLibrary(handle);
        std::cout << "Libary freed\n";
#else
        dlclose(handle);
#endif
    }

    // Template to easily cast the function pointer
    template <typename T> static T GetFunction(LibraryHandle handle, const std::string &funcName) {
#if defined(_WIN32)
        return reinterpret_cast<T>(GetProcAddress(handle, funcName.c_str()));
#else
        return reinterpret_cast<T>(dlsym(handle, funcName.c_str()));
#endif
    }
};

class IGamePlugin {
  public:
    virtual ~IGamePlugin() {}
    virtual void OnLoad() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnUnload() = 0;
};

typedef IGamePlugin *(*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(IGamePlugin *);

class PluginManager {
  private:
    LibraryHandle m_handle = nullptr;
    IGamePlugin *m_plugin = nullptr;
    DestroyPluginFunc m_destroyer = nullptr;
    std::string m_tempPath;

  public:
    PluginManager() {}
    ~PluginManager() { UnloadCurrentPlugin(); }

    void LoadPlugin(const std::string &originalPath) {
        // Unload existing if needed
        if (m_plugin) {
            m_plugin->OnUnload();
            m_destroyer(m_plugin);
            LibraryLoader::Unload(m_handle);
        }

        // COPY the file to a temp path (Pseudo-code)
        // std::filesystem::copy_file(originalPath, tempPath, overwrite_existing);
        std::string loadPath = originalPath; // In production, use the temp path here!

        // Load Library
        m_handle = LibraryLoader::Load(loadPath);
        if (!m_handle)
            return;

        // Locate Factory Functions
        auto creator = LibraryLoader::GetFunction<CreatePluginFunc>(m_handle, "CreatePlugin");
        m_destroyer = LibraryLoader::GetFunction<DestroyPluginFunc>(m_handle, "DestroyPlugin");

        // Create Instance
        if (creator && m_destroyer) {
            m_plugin = creator();
            m_plugin->OnLoad();
        }
    }

    void Update(float dt) {
        if (m_plugin)
            m_plugin->OnUpdate(dt);
    }

    void UnloadCurrentPlugin() {
        // Notify the plugin it is about to die
        if (m_plugin) {
            m_plugin->OnUnload();
        }

        // Destroy the C++ Object
        // We must use the function pointer from the DLL to delete it
        // because the DLL might use a different memory allocator than the engine.
        if (m_destroyer && m_plugin) {
            m_destroyer(m_plugin);
        }

        // Reset pointers
        m_plugin = nullptr;
        m_destroyer = nullptr;

        // Unload the OS Library
        if (m_handle) {
            LibraryLoader::Unload(m_handle);
            m_handle = nullptr;
        }

        // Windows Specific: Delete the temp file
        // You cannot delete the DLL while it is loaded (step 3 must happen first)
        if (!m_tempPath.empty()) {
#if defined(_WIN32)
            // Try to delete the temp file.
            // Sometimes virus scanners or delays keep the file locked for a few ms
            // after FreeLibrary, so a simple retry loop or ignoring failure is common here.
            std::remove(m_tempPath.c_str());
#endif
            m_tempPath.clear();
        }
    }
};

namespace Roar {

extern std::vector<std::string> g_Plugins;

namespace PluginSystem {

void Startup();
void Shutdown();

} // namespace PluginSystem

} // namespace Roar
