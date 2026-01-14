#include "RoarEngine.h"

bool running = true;

void CustomTraceLog(int msgType, const char *text, va_list args) {
    // Calculate the length of the final string
    // We must copy args because vsnprintf "consumes" the list
    va_list args_copy;
    va_copy(args_copy, args);
    int len = std::vsnprintf(nullptr, 0, text, args_copy);
    va_end(args_copy);

    if (len < 0)
        return;

    // Allocate a buffer to hold the formatted string
    // (len + 1 for the null terminator)
    std::vector<char> buffer(len + 1);

    // Format the string into the buffer
    std::vsnprintf(buffer.data(), buffer.size(), text, args);

    // Log the resolved string
    // We use "{}", buffer.data() to be safe in case the message contains brace characters
    switch (msgType) {
    case LOG_INFO:
        spdlog::info("{}", buffer.data());
        break;
    case LOG_ERROR:
        spdlog::error("{}", buffer.data());
        break;
    case LOG_WARNING:
        spdlog::warn("{}", buffer.data());
        break;
    case LOG_DEBUG:
        spdlog::debug("{}", buffer.data());
        break;
    default:
        break;
    }
}

namespace Roar {

void StopApp() { running = false; }
bool IsAppRunning() { return running; }

void AppRun(AppData appdata) {
    SetTraceLogCallback(CustomTraceLog);

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
