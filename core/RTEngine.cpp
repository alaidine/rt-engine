#include "Application.h"
#include "Layer.h"
#include "WindowEvents.h"
#include "RenderSystem.h"
#include "ECS.h"

class SceneLayer : public rt::Layer {
  public:
    SceneLayer() {
        mScene.Init();

        mScene.RegisterComponent<Transform2D>();
        mScene.RegisterComponent<RectangleShape>();

        mRenderSystem = mScene.RegisterSystem<RenderSystem>();
        {
            Signature signature;
            signature.set(mScene.GetComponentType<RectangleShape>());
            signature.set(mScene.GetComponentType<Transform2D>());
            mScene.SetSystemSignature<RenderSystem>(signature);
        }
        mRenderSystem->Init(rt::Application::Get().mRenderer);

        auto entity = mScene.CreateEntity();
        mScene.AddComponent(entity, RectangleShape{
            .color = {1.0f, 1.0f, 1.0f}
        });
        mScene.AddComponent(entity, Transform2D{
            {0.0f, 0.0f},
            {10.0f, 10.0f}
        });
    }
    ~SceneLayer() {}

    void OnEvent(rt::Event &event) override {
        rt::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<rt::WindowClosedEvent>([this](rt::WindowClosedEvent &e) {
            rt::Application::Get().mRenderer->prepared = false;
            return false;
        });
    }

    void OnUpdate(float st) override {
        mRenderSystem->Update(mScene, st);
    }

    void OnRender() override {}

  private:
    Scene mScene;
    std::shared_ptr<RenderSystem> mRenderSystem;
};

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR, _In_ int) {
    for (int32_t i = 0; i < __argc; i++) {
        rt::VulkanBase::args.push_back(__argv[i]);
    };

    rt::ApplicationSpecification spec;
    spec.Title = "RTEngine";
    spec.Name = "RTEngine";

    rt::Application samples(spec, hInstance);
    samples.PushLayer<SceneLayer>();
    samples.Run();
}
