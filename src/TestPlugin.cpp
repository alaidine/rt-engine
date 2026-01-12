#include "PluginManager.h"
#include "IPlugin.h"

#include <iostream>

namespace Roar {

class TestPlugin : public ITestPlugin {
  public:
    void OnLoad() override { std::cout << "Test Plugin Loaded" << std::endl; }
    void OnUnload() override { std::cout << "Test Plugin Unloaded!" << std::endl; }
    const char *GetID() const override { return "TestPlugin"; }
};

} // namespace Roar

extern "C" {
PLUGIN_EXPORT Roar::IPlugin *CreatePlugin() { return new Roar::TestPlugin(); }
}
