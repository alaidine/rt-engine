#include "RoarEngine.h"

namespace Roar {

void AppRun(AppData appdata) {
    appdata.init();
    InitWindow(appdata.width, appdata.height, appdata.name.c_str());

    while (!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        BeginDrawing();
        appdata.frame();
        EndDrawing();
    }
}

};
