#include "EditorLayer.h"
#include "Application.h"
#include "ProjectManager.h"
#include "SceneLayer.h"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

namespace Roar {

bool Quit = false;

// DPI scaling functions
float ScaleToDPIF(float value) { return GetWindowScaleDPI().x * value; }

int ScaleToDPII(int value) { return int(GetWindowScaleDPI().x * value); }

class DocumentWindow {
  public:
    bool Open = false;

    RenderTexture ViewTexture;

    virtual void Setup() = 0;
    virtual void Shutdown() = 0;
    virtual void Show() = 0;
    virtual void Update() = 0;

    bool Focused = false;

    Rectangle ContentRect = {0};
};

class SceneViewWindow : public DocumentWindow {
  public:
    Camera3D Camera = {0};

    void Setup() override {
        ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

        Camera.fovy = 45;
        Camera.up.y = 1;
        Camera.position.y = 3;
        Camera.position.z = -25;

        Image img = GenImageChecked(ScaleToDPII(256), ScaleToDPII(256), ScaleToDPII(32), ScaleToDPII(32), DARKGRAY, WHITE);
        GridTexture = LoadTextureFromImage(img);
        UnloadImage(img);
        GenTextureMipmaps(&GridTexture);
        SetTextureFilter(GridTexture, TEXTURE_FILTER_ANISOTROPIC_16X);
        SetTextureWrap(GridTexture, TEXTURE_WRAP_CLAMP);
    }

    void Shutdown() override {
        UnloadRenderTexture(ViewTexture);
        UnloadTexture(GridTexture);
    }

    void Show() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSizeConstraints(ImVec2(ScaleToDPIF(400.0f), ScaleToDPIF(400.0f)),
                                            ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

        if (ImGui::Begin("3D View", &Open, ImGuiWindowFlags_NoScrollbar)) {
            Focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
            // draw the view
            rlImGuiImageRenderTextureFit(&ViewTexture, true);
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Update() override {
        if (!Open)
            return;

        if (IsWindowResized()) {
            UnloadRenderTexture(ViewTexture);
            ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        }

        float period = 10;
        float magnitude = 25;

        Camera.position.x = sinf(float(GetTime() / period)) * magnitude;

        BeginTextureMode(ViewTexture);
        ClearBackground(SKYBLUE);

        BeginMode3D(Camera);

        // grid of cube trees on a plane to make a "world"
        DrawPlane(Vector3{0, 0, 0}, Vector2{50, 50}, BEIGE); // simple world plane
        float spacing = 4;
        int count = 5;

        for (float x = -count * spacing; x <= count * spacing; x += spacing) {
            for (float z = -count * spacing; z <= count * spacing; z += spacing) {
                Vector3 pos = {x, 0.5f, z};

                Vector3 min = {x - 0.5f, 0, z - 0.5f};
                Vector3 max = {x + 0.5f, 1, z + 0.5f};

                DrawCube(Vector3{x, 1.5f, z}, 1, 1, 1, GREEN);
                DrawCube(Vector3{x, 0.5f, z}, 0.25f, 1, 0.25f, BROWN);
            }
        }

        EndMode3D();
        EndTextureMode();
    }

    Texture2D GridTexture = {0};
};

SceneViewWindow SceneView;

void DrawMenu() {
    ProjectManagerLayer *projectManager = Application::Get().GetLayer<ProjectManagerLayer>();

    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit"))
                Quit = true;
            if (ImGui::MenuItem("Build Project"))
                projectManager->BuildProject();
            if (ImGui::MenuItem("Export Project"))
                projectManager->ExportProject();   
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

EditorLayer::EditorLayer() {
    SetWindowSize(1280.0f, 720.0f);
    Open = true;

    SceneManager *sceneManger = Application::Get().GetLayer<SceneManager>();
    ProjectManagerLayer *projectManager = Application::Get().GetLayer<ProjectManagerLayer>();
    std::string scenePath = projectManager->scenePath.string();
    sceneManger->LoadScene(scenePath);
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnEvent(Event &event) {}

void EditorLayer::OnUpdate(float st) {}

void EditorLayer::OnRender() {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiStyle &style = ImGui::GetStyle();

    ClearBackground(Color{uint8_t(clear_color.x * 255), uint8_t(clear_color.y * 255), uint8_t(clear_color.z * 255),
                          uint8_t(clear_color.w * 255)});
    BeginDrawing();
    rlImGuiBegin();
    DrawMenu();
    style.WindowRounding = 10.0f; // Set rounding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(ScaleToDPIF(400.0f), ScaleToDPIF(400.0f)),
                                        ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
    if (ImGui::Begin("Viewport", &Open, ImGuiWindowFlags_NoScrollbar)) {
        Focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
        // draw the view
        auto sm = Application::Get().GetLayer<SceneManager>();
        rlImGuiImageRenderTextureFit(&sm->ViewTexture, true);
    }
    ImGui::End();
    ImGui::PopStyleVar();
    rlImGuiEnd();
    EndDrawing();
}

} // namespace Roar
