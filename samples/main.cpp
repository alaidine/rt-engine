#include "Application.h"
#include "Layer.h"
#include "WindowEvents.h"

class SamplesApp : public rt::Layer {
  public:
    SamplesApp() {}
    ~SamplesApp() {}

    void OnEvent(rt::Event &event) override {
        std::cout << event.ToString() << std::endl;

        rt::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<rt::WindowClosedEvent>([this](rt::WindowClosedEvent &e) {
            rt::Application::Get().mRenderer->prepared = false;
            return false;
        });
    }

    void OnUpdate(float st) override {}
    void OnRender() override {
        rt::Rectangle rect = {0, 0, 100, 100};
        rt::Application::Get().mRenderer->DrawRect(rect, {1.0f, 1.0f, 1.0f});
    }
};

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR, _In_ int) {
    for (int32_t i = 0; i < __argc; i++) {
        rt::VulkanBase::args.push_back(__argv[i]);
    };

    rt::ApplicationSpecification spec;
    spec.Title = "RTEngine Samples";
    spec.Name = "RTEngineSamples";

    rt::Application samples(spec, hInstance);
    samples.PushLayer<SamplesApp>();
    samples.Run();
}
