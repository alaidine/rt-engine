#include "Networking.h"

extern "C" {
#if defined(_WIN32)
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
void *
CreatePlugin() {
    return new NetClient();
}

#if defined(_WIN32)
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
    void DestroyPlugin(NetClient* ptr) {
    delete ptr;
}
