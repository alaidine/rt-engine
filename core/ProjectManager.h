#pragma once

#include "Layer.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Roar {

class ProjectManagerLayer : public Layer {
  public:
    void OnEvent(Event &event) override;
    void OnUpdate(float ts) override;
    void OnRender() override;

    bool CreateProject();
    bool OpenProject();

    void BuildProject();
    void ExportProject();
    
    std::filesystem::path projectRoot;
    std::filesystem::path scenePath;
    std::filesystem::path gameLibPath;
};

} // namespace Roar
