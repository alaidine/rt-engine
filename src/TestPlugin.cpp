#include "PluginManager.h"
#include <iostream>

class TestPlugin : public IGamePlugin {
  public:
    void OnLoad() override { std::cout << "Test Plugin Loaded" << std::endl; }

    void OnUpdate(float dt) override {
    }

    void OnUnload() override { std::cout << "Test Plugin Unloaded!" << std::endl; }
};

extern "C" {
#if defined(_WIN32)
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
IGamePlugin *
CreatePlugin() {
    return new TestPlugin();
}

#if defined(_WIN32)
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
    void DestroyPlugin(IGamePlugin* ptr) {
    delete ptr;
}
}
