#pragma once

#include "IPlugin.h"
#include "RoarEngine.h"

namespace Roar {
namespace Physics {

class IPhysics : public IPlugin {
  public:
    virtual void InitDemo(uint32_t width, uint32_t height) = 0;
    virtual void UpdateDemo() = 0;
    virtual void CleanupDemo() = 0;

    virtual void Startup() = 0;
    virtual void Step() = 0;
    virtual void Shutdown() = 0;
};

} // namespace Physics
} // namespace Roar
