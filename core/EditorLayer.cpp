#include "EditorLayer.h"
#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

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

void DoMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit"))
                Quit = true;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("3D View", nullptr, &SceneView.Open);

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

EditorLayer::EditorLayer() {
    rlImGuiSetup(true);
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    SceneView.Setup();
    SceneView.Open = true;

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // tell ImGui the display scale
    if (!IsWindowState(FLAG_WINDOW_HIGHDPI)) {
        io.DisplayFramebufferScale.x = GetWindowScaleDPI().x;
        io.DisplayFramebufferScale.y = GetWindowScaleDPI().y;
    }

    static constexpr int DefaultFonSize = 13;
    ImFontConfig defaultConfig;
    defaultConfig.SizePixels = DefaultFonSize;

    if (!IsWindowState(FLAG_WINDOW_HIGHDPI)) {
        defaultConfig.SizePixels = defaultConfig.SizePixels * GetWindowScaleDPI().y;
        defaultConfig.RasterizerMultiply = GetWindowScaleDPI().y;
    }

    defaultConfig.PixelSnapH = true;
    io.Fonts->AddFontDefault(&defaultConfig);
}

EditorLayer::~EditorLayer() {
    rlImGuiShutdown();
    SceneView.Shutdown();
}

void EditorLayer::OnEvent(Roar::Event &event) {}

void EditorLayer::OnUpdate(float st) { SceneView.Update(); }

void EditorLayer::OnRender() {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ClearBackground(Color{uint8_t(clear_color.x * 255), uint8_t(clear_color.y * 255), uint8_t(clear_color.z * 255),
                          uint8_t(clear_color.w * 255)});

    rlImGuiBegin();
    DoMainMenu();

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 10.0f; // Set rounding

    if (SceneView.Open)
        SceneView.Show();

    rlImGuiEnd();
}
