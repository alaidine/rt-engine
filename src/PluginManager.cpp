#include "PluginManager.h"

namespace Roar {

std::vector<std::string> g_Plugins;
PluginRegistry *registry = nullptr;

namespace PluginSystem {

void AddPlugin(std::string pluginName) { g_Plugins.push_back(pluginName); }

void Startup() {
    registry = new PluginRegistry;
    registry->LoadLibs(g_Plugins);
}

void Shutdown() { registry->Shutdown(); }
} // namespace PluginSystem

LibraryHandle LibraryLoader::Load(const std::string &path) {
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

void LibraryLoader::Unload(LibraryHandle handle) {
    if (!handle)
        return;
#if defined(_WIN32)
    FreeLibrary(handle);
    std::cout << "Library freed\n";
#else
    dlclose(handle);
#endif
}

// Template to easily cast the function pointer
template <typename T> T LibraryLoader::GetFunction(LibraryHandle handle, const std::string &funcName) {
#if defined(_WIN32)
    return reinterpret_cast<T>(GetProcAddress(handle, funcName.c_str()));
#else
    return reinterpret_cast<T>(dlsym(handle, funcName.c_str()));
#endif
}

} // namespace Roar
