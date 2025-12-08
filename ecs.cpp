#include "Core.hpp"
#include "System/GravitySystem.hpp"
#include "System/RendererSystem.hpp"
#include "System/InputControllerSystem.hpp"
#include "Builder/Builder.hpp"
#include "Prefab.hpp"
#include <stdlib.h>
#include <time.h>

Core _core;

int main() {
    _core.Init();

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "TEST");
    
    MiniBuilder::RegisterComponentBuilder registerTest;
    registerTest.RegisterComponents<Position, Gravity, Velocity, Sprite, InputController>(_core);

    auto gravitySystem = _core.RegisterSystem<GravitySystem>();
    gravitySystem->order = 2;
    auto renderSystem = _core.RegisterSystem<RendererSystem>();
    renderSystem->order = 3;
    auto inputControllerSystem = _core.RegisterSystem<InputControllerSystem>();
    inputControllerSystem->order = 1;

    Signature gravitySignature;
    Signature RenderSignature;
    Signature inputControllerSignature;

    MiniBuilder::SystemBuilder gravityBuilder(gravitySignature);
    gravityBuilder.BuildSignature<GravitySystem, Gravity, Position>(_core);

    MiniBuilder::SystemBuilder rendererBuilder(RenderSignature);
    rendererBuilder.BuildSignature<RendererSystem, Position, Sprite>(_core);

    MiniBuilder::SystemBuilder inputControllerBuilder(inputControllerSignature);
    inputControllerBuilder.BuildSignature<InputControllerSystem, Position, Velocity, InputController>(_core);

    Entity player = Prefab::MakePlayer(_core, (float)screenWidth/2, (float)screenHeight/2);
    
    srand(time(NULL));

    Entity enemy1[5000];

    for (int i = 0; i < 5000; i++)
        enemy1[i] = Prefab::MakeEnemy(_core, rand() % 800 + 1, rand() % 450+ 1);
 
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        BeginDrawing();
        
        ClearBackground(WHITE);
        DrawText("Test text", 10, 10, 20, BLACK);
        
        _core.UpdateAllSystem();
        
        EndDrawing();
    }
    

    CloseWindow();

    return 0;
}