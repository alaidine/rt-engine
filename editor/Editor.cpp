#include "Application.h"
#include "EditorLayer.h"

int main(int argc, char *argv[]) {
    Roar::ApplicationSpecification spec;
    spec.Title = "RoarEditor";
    spec.Name = "RoarEditor";

    Roar::Application samples(spec);
    samples.PushLayer<EditorLayer>();
    samples.Run();
}
