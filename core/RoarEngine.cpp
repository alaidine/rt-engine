#include "Application.h"
#include "SceneLayer.h"

int main(int argc, char *argv[]) {
    Roar::ApplicationSpecification spec;
    spec.Title = "RoarEngine";
    spec.Name = "RoarEngine";

    Roar::Application samples(spec);
    samples.PushLayer<SceneLayer>();
    samples.Run();
}
