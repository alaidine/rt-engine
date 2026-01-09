#include "RoarEngine.h"

namespace Roar {

void AppRun(AppData appdata) {
    appdata.init();

    InitWindow(appdata.width, appdata.height, appdata.name.c_str());

    PluginSystem::Startup();

    while (!WindowShouldClose()) {
        ClearBackground(RAYWHITE);

        BeginDrawing();
        
        appdata.frame();

        EndDrawing();
    }

    PluginSystem::Shutdown();
}

};
