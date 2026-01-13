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
};

class Box2DPhysics : public IPhysics {
  public:
    void InitDemo(uint32_t width, uint32_t height) override;
    void UpdateDemo() override;
    void CleanupDemo() override;

    const char *GetID() const override { return "Box2DPhysics"; }
};

} // namespace Physics

} // namespace Roar
