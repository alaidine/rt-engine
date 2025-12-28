#include "SceneLayer.h"

SceneLayer::SceneLayer() {
    mScene.Init();

    mScene.RegisterComponent<Transform2D>();
    mScene.RegisterComponent<RectangleShape>();
    mScene.RegisterComponent<ScriptComponent>();

    mRenderSystem = mScene.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(mScene.GetComponentType<RectangleShape>());
        signature.set(mScene.GetComponentType<Transform2D>());
        mScene.SetSystemSignature<RenderSystem>(signature);
    }
    mRenderSystem->Init();

    auto entity = mScene.CreateEntity();
    mScene.AddComponent(entity, RectangleShape{.color = {230, 41, 55, 255}});
    mScene.AddComponent(entity, Transform2D{{0.0f, 0.0f}, {10.0f, 10.0f}});
}

SceneLayer::~SceneLayer() {}

void SceneLayer::OnEvent(Roar::Event &event) {}

void SceneLayer::OnUpdate(float st) { mRenderSystem->Update(mScene, st); }

void SceneLayer::OnRender() {}
