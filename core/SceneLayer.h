#include "Common.h"
#include "Layer.h"
#include "RenderSystem.h"
#include "ScriptSystem.h"
#include "RoarConfig.h"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

namespace Roar {

class SceneManager : public Layer {
  public:
    SceneManager();
    ~SceneManager();

    void OnEvent(Roar::Event &event) override;
    void OnUpdate(float st) override;
    void OnRender() override;

    void PauseUnpause();
    void LoadScene(std::string scenePath);

    RenderTexture ViewTexture;

    bool paused = true;

  private:
    Ref<Scene> mScene;
    RoarConfig config;
    std::shared_ptr<RenderSystem> mRenderSystem;
    std::shared_ptr<ScriptSystem> mScriptSystem;
};

class SceneLayer : public Layer {
  public:
    SceneLayer();
    ~SceneLayer();

    void OnEvent(Roar::Event &event) override;
    void OnUpdate(float st) override;
    void OnRender() override;

  private:
    Ref<Scene> mScene;
    std::shared_ptr<RenderSystem> mRenderSystem;
    std::shared_ptr<ScriptSystem> mScriptSystem;
    RoarConfig config;
};

} // namespace Roar
