// #include "Entrypoint.h"
#include "Application.h"
#include "Networking.h"
#include "PluginManager.h"

namespace Roar {

typedef struct AppData {
    std::string name = "name";
    uint32_t width = 1280;
    uint32_t height = 720;
    void (*init)();
    void (*frame)();
} AppData;

void AppRun(AppData appdata);

}
