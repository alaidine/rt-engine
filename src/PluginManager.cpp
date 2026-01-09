#include "PluginManager.h"

#include <string>
#include <vector>

namespace Roar {
std::vector<PluginManager *> plugins;
std::vector<std::string> g_Plugins;

namespace PluginSystem {
void Startup() {
    for (auto name : g_Plugins) {
        plugins.push_back(new PluginManager);
        plugins.back()->LoadPlugin(name);
    }
}

void Shutdown() {
    for (const auto plug : plugins) {
        delete plug;
    }
}
} // namespace PluginSystem

} // namespace Roar
