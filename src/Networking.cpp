#include "Networking.h"

extern "C" {
PLUGIN_EXPORT Roar::IPlugin *CreatePlugin() { return new Roar::NetlibNetwork(); }
}
