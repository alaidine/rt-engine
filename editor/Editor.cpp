#include "Application.h"
#include "Common.h"
#include "EditorLayer.h"
#include "ProjectManager.h"
#include "SceneLayer.h"

#include "imgui.h"
#include "nfd.h"
#include "raylib.h"
#include "rlImGui.h"

namespace Roar {

class HubLayer : public Layer {
  public:
    // DPI scaling functions
    float ScaleToDPIF(float value) { return GetWindowScaleDPI().x * value; }
    int ScaleToDPII(int value) { return int(GetWindowScaleDPI().x * value); }

    HubLayer() { SetWindowSize(400, 650); }
    ~HubLayer() {}

    void OnEvent(Event &event) override {}
    void OnUpdate(float ts) override {}
    void OnRender() override {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        rlImGuiBegin();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        bool p_open = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        if (!ImGui::Begin("RoarEngine Hub", &p_open, window_flags)) {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }

        ImGui::SetCursorPos(ImVec2((GetScreenWidth() - 200) / 2, (GetScreenHeight() - 25) / 2));

        if (ImGui::Button("Open existing project", ImVec2(200, 20))) {
            if (Application::Get().GetLayer<ProjectManagerLayer>()->OpenProject()) {
                TransitionTo<EditorLayer>();
            }
            ImGui::End();
            ImGui::PopStyleVar();
            rlImGuiEnd();
            EndDrawing();
            return;
        }

        ImGui::SetCursorPos(ImVec2((GetScreenWidth() - 200) / 2, (GetScreenHeight() + 25) / 2));

        if (ImGui::Button("Create new project", ImVec2(200, 20))) {
            if (Application::Get().GetLayer<ProjectManagerLayer>()->CreateProject()) {
                TransitionTo<EditorLayer>();
            }
            ImGui::End();
            ImGui::PopStyleVar();
            rlImGuiEnd();
            EndDrawing();
            return;
        }

        ImGui::End();
        ImGui::PopStyleVar();

        rlImGuiEnd();

        EndDrawing();
    }

  private:
    enum HubState { OPEN_PROJECT, CREATE_PROJECT };
};

} // namespace Roar

int main(int argc, char *argv[]) {
    Roar::ApplicationSpecification spec;
    spec.Title = "RoarEditor";
    spec.Name = "RoarEditor";
    spec.isEditor = true;
    spec.WindowSpec.Width = 400;
    spec.WindowSpec.Height = 650;

    Roar::Application editor(spec);
    editor.PushLayer<Roar::SceneManager>();
    editor.PushLayer<Roar::ProjectManagerLayer>();
    editor.PushLayer<Roar::HubLayer>();
    editor.Run();
}
