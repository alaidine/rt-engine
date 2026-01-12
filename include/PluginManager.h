#pragma once

#include "framework.h"

#include "IPlugin.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
#define _WINSOCKAPI_
#include <windows.h>
typedef HMODULE LibraryHandle;
#else
#include <dlfcn.h>
typedef void *LibraryHandle;
#endif
#include <RoarEngine.h>

#if defined(_MSC_VER)
#define PLUGIN_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#else
#define PLUGIN_EXPORT
#endif

namespace Roar {

class LibraryLoader {
  public:
    static LibraryHandle Load(const std::string &path);
    static void Unload(LibraryHandle handle);
    template <typename T> static T GetFunction(LibraryHandle handle, const std::string &funcName);
};

class ITestPlugin : public IPlugin {
  public:
    virtual void OnLoad() = 0;
    virtual void OnUnload() = 0;
};

typedef IPlugin *(*CreatePluginFunc)();

class PluginRegistry {
    std::unordered_map<std::string, IPlugin *> m_plugins;
    std::vector<LibraryHandle> m_loadedLibraries;

  public:
    void LoadAll(const std::string &directoryPath) {
        namespace fs = std::filesystem;

        for (const auto &entry : fs::directory_iterator(directoryPath)) {
            // Check for .dll or .so extension
            if (entry.path().extension() == ".dll" || entry.path().extension() == ".so") {
                LoadSingle(entry.path().string());
            }
        }
    }

    void LoadLibs(std::vector<std::string> names) {
        for (const auto &entry : names) {
            LoadSingle(entry);
        }
    }

    void LoadSingle(const std::string &path) {
        LibraryHandle handle;

#if defined(_WIN32)
        handle = LibraryLoader::Load(path + ".dll");
#else
        handle = LibraryLoader::Load(path + ".so");
#endif

        if (!handle)
            return;

        auto creator = LibraryLoader::GetFunction<CreatePluginFunc>(handle, "CreatePlugin");

        if (creator) {
            IPlugin *plugin = creator();

            std::string id = plugin->GetID();

            if (m_plugins.find(id) == m_plugins.end()) {
                m_plugins[id] = plugin;
                m_loadedLibraries.push_back(handle);
                std::cout << "Registered Plugin: " << id << std::endl;
            } else {
                std::cerr << "Conflict: Plugin " << id << " already loaded!" << std::endl;
            }
        }
    }

    // The user asks for a specific Interface (T) and the ID string
    template <typename T> T *GetSystem(const std::string &id) {
        auto it = m_plugins.find(id);
        if (it != m_plugins.end()) {
            // Downcast the IPlugin* to the specific
            // dynamic_cast is safer but requires RTTI to work across DLLs (can be tricky)
            // static_cast is faster but assumes you know what you are doing.
            return static_cast<T *>(it->second);
        }
        return nullptr;
    }

    void Shutdown() {
        for (auto &pair : m_plugins) {
            delete pair.second; // Virtual destructor handles specific cleanup
        }
        m_plugins.clear();
        for (auto handle : m_loadedLibraries) {
            LibraryLoader::Unload(handle);
        }
        m_loadedLibraries.clear();
    }
};

extern std::vector<std::string> g_Plugins;

PluginRegistry *GetRegistry();

namespace PluginSystem {

void AddPlugin(std::string pluginName);
void Startup();
void Shutdown();

} // namespace PluginSystem

} // namespace Roar
