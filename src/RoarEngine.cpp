#include "RoarEngine.h"

bool running = true;

static void CustomTraceLog(int msgType, const char *text, va_list args) {
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
        ROAR_CORE_INFO("{}", buffer.data());
        break;
    case LOG_ERROR:
        ROAR_CORE_ERROR("{}", buffer.data());
        break;
    case LOG_WARNING:
        ROAR_CORE_WARN("{}", buffer.data());
        break;
    case LOG_DEBUG:
        ROAR_CORE_DEBUG("{}", buffer.data());
        break;
    default:
        break;
    }
}

namespace Roar {

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init() {
    spdlog::set_pattern("%^[%H:%M:%S %z] [%L] %n: %v%$");

    s_CoreLogger = spdlog::stdout_color_mt("ROAR");
    s_CoreLogger->set_level(spdlog::level::trace);

    s_ClientLogger = spdlog::stdout_color_mt("APP");
    s_ClientLogger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> &Log::GetCoreLogger() { return s_CoreLogger; }

std::shared_ptr<spdlog::logger> &Log::GetClientLogger() { return s_ClientLogger; }

void StopApp() { running = false; }

bool IsAppRunning() { return running; }

void AppRun(AppData appdata) {
    Log::Init();

    SetTraceLogCallback(CustomTraceLog);

    if (!appdata.headless)
        InitWindow(appdata.width, appdata.height, appdata.name == nullptr ? "Name" : appdata.name);

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
