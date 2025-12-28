#include "Application.h"
#include "Layer.h"
#include "WindowEvents.h"

class SamplesApp : public Roar::Layer {
  public:
    SamplesApp() {}
    ~SamplesApp() {}

    void OnEvent(Roar::Event &event) override {}

    void OnUpdate(float st) override {}
    void OnRender() override {}
};

int main(int argc, char *argv[]) {
    Roar::ApplicationSpecification spec;
    spec.Title = "RoarEngine Samples";
    spec.Name = "RoarEngineSamples";

    Roar::Application samples(spec);
    samples.PushLayer<SamplesApp>();
    samples.Run();
}
