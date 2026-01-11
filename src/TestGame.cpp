#include "RoarEngine.h"

struct {
    int x = 0;
    int y = 0;
} state;

void init() {
    Roar::PluginSystem::AddPlugin("networking");
    Roar::PluginSystem::Startup();
}

void frame() {}

void cleanup() { Roar::PluginSystem::Shutdown(); }

int main(int argc, char *argv) { Roar::AppRun(Roar::AppData{.init = init, .frame = frame, .cleanup = cleanup}); }
