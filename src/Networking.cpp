#include "Networking.h"
#include "IPlugin.h"

extern "C" {
PLUGIN_EXPORT Roar::IPlugin *CreatePlugin() { return new Roar::NetlibNetwork(); }
}
