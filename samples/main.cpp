#include "renderer.h"

rt::Renderer *renderer;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (renderer != NULL) {
    renderer->handleMessages(hWnd, uMsg, wParam, lParam);
  }
  return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR, _In_ int) {
  renderer = new rt::Renderer();

  rt::Texture texture;
  rt::Rectangle rect = {0, 364, 256, 256}; // Example dimensions
  glm::vec2 position = {0, 0};             // Example position
  rt::Color tint = {255, 255, 255, 255};   // White tint

  renderer->initVulkan();
  renderer->setupWindow(hInstance, WndProc);
  renderer->prepare();
  renderer->init();

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

    renderer->BeginDrawing();

    renderer->DrawTextureRec(texture, rect, position, tint);

    renderer->EndDrawing();
  }

  delete renderer;
}
