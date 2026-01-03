#include "Application.h"
#include "SceneLayer.h"

int main(int argc, char *argv[]) {
    Roar::ApplicationSpecification spec;
    spec.Title = "RoarEngine";
    spec.Name = "RoarEngine";
    spec.isEditor = false;

    Roar::Application samples(spec);
    samples.PushLayer<Roar::SceneLayer>();
    samples.Run();
}
