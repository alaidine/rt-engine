#include "RoarEngine.h"

bool running = true;

namespace Roar {

void StopApp() { running = false; }
bool IsAppRunning() { return running; }

void AppRun(AppData appdata) {
    if (!appdata.headless)
        InitWindow(appdata.width, appdata.height, appdata.name.c_str());

    appdata.init();

    while (IsAppRunning()) {
        if (appdata.headless) {
            appdata.frame();
        } else {
            if (WindowShouldClose()) {
                StopApp();
            }
            ClearBackground(RAYWHITE);
            BeginDrawing();
            appdata.frame();
            EndDrawing();
        }
    }

    if (!appdata.headless)
        CloseWindow();
}

}; // namespace Roar
