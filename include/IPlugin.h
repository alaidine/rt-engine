#pragma once

namespace Roar {

class IPlugin {
  public:
    virtual ~IPlugin() = default;
    virtual const char *GetID() const = 0;
};

} // namespace Roar
