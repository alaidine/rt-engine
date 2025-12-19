#include "renderer.h"

rt::VulkanRenderer *renderer;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (renderer != NULL) {
        renderer->handleMessages(hWnd, uMsg, wParam, lParam);
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR, _In_ int) {
    for (int32_t i = 0; i < __argc; i++) {
        rt::VulkanBase::args.push_back(__argv[i]);
    };

    renderer = new rt::VulkanRenderer();
    renderer->initVulkan();
    renderer->setupWindow(hInstance, WndProc);
    renderer->prepare();
    renderer->Init();

    rt::Rectangle rect = {0, 0, 50, 50}; // Top-left quadrant (should show top-left of image)

    MSG msg;
    bool quitMessageReceived = false;
    while (!quitMessageReceived) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                quitMessageReceived = true;
                break;
            }
        }

        renderer->StartDrawing();

        renderer->DrawTextureRec(rect, {0, 0});

        renderer->EndDrawing();
    }

    delete renderer;
}
