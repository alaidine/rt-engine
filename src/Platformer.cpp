#include "IPhysics.h"
#include "RoarEngine.h"

constexpr uint32_t WIDTH = 1920;
constexpr uint32_t HEIGHT = 1080;

Roar::Physics::IPhysics *phys = nullptr;

static void init() {
    Roar::PluginSystem::AddPlugin("PhysicsPlugin");
    Roar::PluginSystem::Startup();

    phys = Roar::GetRegistry()->GetSystem<Roar::Physics::IPhysics>("Box2DPhysics");
    phys->InitDemo(WIDTH, HEIGHT);
}

static void frame() { phys->UpdateDemo(); }

static void cleanup() { phys->CleanupDemo(); }

int main(int argc, char *argv) {
    Roar::AppRun(Roar::AppData{.name = "Platformer Game",
                               .width = WIDTH,
                               .height = HEIGHT,
                               .headless = false,
                               .init = init,
                               .frame = frame,
                               .cleanup = cleanup});
}
