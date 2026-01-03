#include "SceneLayer.h"
#include "Common.h"
#include "Scripting.h"

namespace Roar {

SceneLayer::SceneLayer() {
    mScene = CreateRef<Scene>();

    mScene->Init();

    Roar::Scripting::SetScene(mScene);

    mScene->RegisterComponent<TransformComponent>();
    mScene->RegisterComponent<RectangleComponent>();
    mScene->RegisterComponent<ScriptComponent>();

    mRenderSystem = mScene->RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(mScene->GetComponentType<RectangleComponent>());
        signature.set(mScene->GetComponentType<TransformComponent>());
        mScene->SetSystemSignature<RenderSystem>(signature);
    }
    mRenderSystem->Init();

    mScriptSystem = mScene->RegisterSystem<ScriptSystem>();
    {
        Signature signature;
        signature.set(mScene->GetComponentType<ScriptComponent>());
        mScene->SetSystemSignature<ScriptSystem>(signature);
    }
    mScriptSystem->Init();

    auto entity = mScene->CreateEntity();
    mScene->AddComponent(entity, RectangleComponent{.color = {230, 41, 55, 255}});
    mScene->AddComponent(entity, TransformComponent{{0.0f, 0.0f}, {10.0f, 10.0f}});

    mScene->AddComponent(entity, ScriptComponent{"Sandbox.Player"});
    Roar::Scripting::OnCreateEntity(entity);
}

SceneLayer::~SceneLayer() {}

void SceneLayer::OnEvent(Roar::Event &event) {}

void SceneLayer::OnUpdate(float st) { mScriptSystem->Update(st); }

void SceneLayer::OnRender() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    mRenderSystem->Update(*mScene);
    EndDrawing();
}

SceneManager::SceneManager() {
    mScene = CreateRef<Scene>();

    mScene->Init();

    Roar::Scripting::SetScene(mScene);

    mScene->RegisterComponent<TransformComponent>();
    mScene->RegisterComponent<RectangleComponent>();
    mScene->RegisterComponent<ScriptComponent>();

    mRenderSystem = mScene->RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(mScene->GetComponentType<RectangleComponent>());
        signature.set(mScene->GetComponentType<TransformComponent>());
        mScene->SetSystemSignature<RenderSystem>(signature);
    }
    mRenderSystem->Init();

    mScriptSystem = mScene->RegisterSystem<ScriptSystem>();
    {
        Signature signature;
        signature.set(mScene->GetComponentType<ScriptComponent>());
        mScene->SetSystemSignature<ScriptSystem>(signature);
    }
    mScriptSystem->Init();

    ViewTexture = LoadRenderTexture(1280.0f, 720.0f);
}

SceneManager::~SceneManager() { UnloadRenderTexture(ViewTexture); }

void SceneManager::OnEvent(Event &event) {}

void SceneManager::OnUpdate(float st) {
    if (paused)
        return;
    mScriptSystem->Update(st);
}

void SceneManager::OnRender() {
    // if (IsWindowResized()) {
    //     UnloadRenderTexture(ViewTexture);
    //     ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    // }
    BeginTextureMode(ViewTexture);
    ClearBackground(RAYWHITE);
    mRenderSystem->Update(*mScene);
    EndTextureMode();
}

void SceneManager::PauseUnpause() { paused = !paused; }

void SceneManager::LoadScene(std::string scenePath) { config.fromFile(mScene, scenePath); }

} // namespace Roar
