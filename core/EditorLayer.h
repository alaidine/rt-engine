#include "Layer.h"

class EditorLayer : public Roar::Layer {
  public:
    EditorLayer();
    ~EditorLayer();

    void OnEvent(Roar::Event &event) override;
    void OnUpdate(float st) override;
    void OnRender() override;
};
