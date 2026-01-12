#pragma once

#include "framework.h"
#include "raylib.h"

#include "Networking.h"
#include "PluginManager.h"

#include "spdlog/spdlog.h"

#if defined(_WIN32)
#define RO_DEBGBRK() __debugbreak()
#endif

#define RO_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define RO_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define RO_LOG_ERR(...) spdlog::error(__VA_ARGS__)
#define RO_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define RO_ASSERT(_EXPR) assert(_EXPR)

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
    std::string name = "name";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool headless = false;
    void (*init)();
    void (*frame)();
    void (*cleanup)();
} AppData;

void AppRun(AppData appdata);
void StopApp();

} // namespace Roar
