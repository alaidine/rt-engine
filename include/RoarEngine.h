#pragma once

#include "Networking.h"
#include "PluginManager.h"
#include "framework.h"
#include "raylib.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>

#if defined(_WIN32)
#if defined(ENGINE_EXPORTS)
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API __attribute__((visibility("default")))
#endif

namespace Roar {

typedef struct AppData {
    const char *name = nullptr;
    uint32_t width = 1280;
    uint32_t height = 720;
    bool headless = false;
    void (*init)();
    void (*frame)();
    void (*cleanup)();
} AppData;

void AppRun(AppData appdata);

void StopApp();

bool IsAppRunning();

class Log {
  public:
    static void Init();
    static std::shared_ptr<spdlog::logger> &GetCoreLogger();
    static std::shared_ptr<spdlog::logger> &GetClientLogger();
  private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

} // namespace Roar

#define ROAR_CORE_TRACE(...) ::Roar::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ROAR_CORE_INFO(...) ::Roar::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ROAR_CORE_WARN(...) ::Roar::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ROAR_CORE_ERROR(...) ::Roar::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ROAR_CORE_DEBUG(...) ::Roar::Log::GetCoreLogger()->debug(__VA_ARGS__)

#define ROAR_TRACE(...) ::Roar::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ROAR_INFO(...) ::Roar::Log::GetClientLogger()->info(__VA_ARGS__)
#define ROAR_WARN(...) ::Roar::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ROAR_ERROR(...) ::Roar::Log::GetClientLogger()->error(__VA_ARGS__)
#define ROAR_DEBUG(...) ::Roar::Log::GetClientLogger()->debug(__VA_ARGS__)
