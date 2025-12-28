#include "Layer.h"
#include "RenderSystem.h"

class SceneLayer : public Roar::Layer {
  public:
    SceneLayer();
    ~SceneLayer();

    void OnEvent(Roar::Event &event) override;
    void OnUpdate(float st) override;
    void OnRender() override;

  private:
    Scene mScene;
    std::shared_ptr<RenderSystem> mRenderSystem;
};
