#include "window.h"

#include "inputevents.h"
#include "windowevents.h"

#include <assert.h>
#include <iostream>

namespace rt {

Window *sWindow;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (sWindow != NULL) {
        sWindow->handleMessages(hWnd, uMsg, wParam, lParam);
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

Window::Window(const WindowSpecification &specification) : mSpecification(specification) { sWindow = this; }

Window::~Window() { Destroy(); }

void Window::PollEvent() {
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            quitMessageReceived = true;
            break;
        }
    }
}

void Window::Create() {}

std::string Window::getWindowTitle() const {
    std::string windowTitle{mSpecification.Name};
    if (!settings.overlay) {
        windowTitle += " - " + std::to_string(mFrameCounter) + " fps";
    }
    return windowTitle;
}

void Window::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /*
    switch (uMsg) {
    case WM_CLOSE:
        prepared = false;
        DestroyWindow(hWnd);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        ValidateRect(window, NULL);
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case KEY_P:
            paused = !paused;
            break;
        case KEY_F1:
            ui.visible = !ui.visible;
            break;
        case KEY_F2:
            if (camera.type == Camera::CameraType::lookat) {
                camera.type = Camera::CameraType::firstperson;
            } else {
                camera.type = Camera::CameraType::lookat;
            }
            break;
        case KEY_ESCAPE:
            PostQuitMessage(0);
            break;
        }

        if (camera.type == Camera::firstperson) {
            switch (wParam) {
            case KEY_W:
                camera.keys.up = true;
                break;
            case KEY_S:
                camera.keys.down = true;
                break;
            case KEY_A:
                camera.keys.left = true;
                break;
            case KEY_D:
                camera.keys.right = true;
                break;
            }
        }

        keyPressed((uint32_t)wParam);
        break;
    case WM_KEYUP:
        if (camera.type == Camera::firstperson) {
            switch (wParam) {
            case KEY_W:
                camera.keys.up = false;
                break;
            case KEY_S:
                camera.keys.down = false;
                break;
            case KEY_A:
                camera.keys.left = false;
                break;
            case KEY_D:
                camera.keys.right = false;
                break;
            }
        }
        break;
    case WM_LBUTTONDOWN:
        mouseState.position = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
        mouseState.buttons.left = true;
        break;
    case WM_RBUTTONDOWN:
        mouseState.position = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
        mouseState.buttons.right = true;
        break;
    case WM_MBUTTONDOWN:
        mouseState.position = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
        mouseState.buttons.middle = true;
        break;
    case WM_LBUTTONUP:
        mouseState.buttons.left = false;
        break;
    case WM_RBUTTONUP:
        mouseState.buttons.right = false;
        break;
    case WM_MBUTTONUP:
        mouseState.buttons.middle = false;
        break;
    case WM_MOUSEWHEEL: {
        short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        camera.translate(glm::vec3(0.0f, 0.0f, (float)wheelDelta * 0.005f));
        break;
    }
    case WM_MOUSEMOVE: {
        handleMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_SIZE:
        if ((prepared) && (wParam != SIZE_MINIMIZED)) {
            if ((resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED))) {
                destWidth = LOWORD(lParam);
                destHeight = HIWORD(lParam);
                windowResize();
            }
        }
        break;
    case WM_GETMINMAXINFO: {
        LPMINMAXINFO minMaxInfo = (LPMINMAXINFO)lParam;
        minMaxInfo->ptMinTrackSize.x = 64;
        minMaxInfo->ptMinTrackSize.y = 64;
        break;
    }
    case WM_ENTERSIZEMOVE:
        resizing = true;
        break;
    case WM_EXITSIZEMOVE:
        resizing = false;
        break;
    }
    */

    WindowClosedEvent event;

    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        PostQuitMessage(0);
        RaiseEvent(event);
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case KEY_ESCAPE:
            PostQuitMessage(0);
            break;
        }
    }

    OnHandleMessage(hWnd, uMsg, wParam, lParam);
}

void Window::OnHandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {};

HWND Window::Create(HINSTANCE hinstance) {
    this->windowInstance = hinstance;

    WNDCLASSEX wndClass{
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hinstance,
        .hIcon = LoadIcon(NULL, IDI_APPLICATION),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH),
        .lpszMenuName = NULL,
        .lpszClassName = mSpecification.Name.c_str(),
        .hIconSm = LoadIcon(NULL, IDI_WINLOGO),
    };

    if (!RegisterClassEx(&wndClass)) {
        std::cout << "Could not register window class!\n";
        fflush(stdout);
        exit(1);
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (settings.fullscreen) {
        if ((mSpecification.Width != (uint32_t)screenWidth) && (mSpecification.Height != (uint32_t)screenHeight)) {
            DEVMODE dmScreenSettings;
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
            dmScreenSettings.dmSize = sizeof(dmScreenSettings);
            dmScreenSettings.dmPelsWidth = mSpecification.Width;
            dmScreenSettings.dmPelsHeight = mSpecification.Height;
            dmScreenSettings.dmBitsPerPel = 32;
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
            if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
                if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error",
                               MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
                    settings.fullscreen = false;
                } else {
                    return nullptr;
                }
            }
            screenWidth = mSpecification.Width;
            screenHeight = mSpecification.Height;
        }
    }

    DWORD dwExStyle;
    DWORD dwStyle;

    if (settings.fullscreen) {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    } else {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    }

    RECT windowRect{.left = 0L,
                    .top = 0L,
                    .right = settings.fullscreen ? (long)screenWidth : (long)mSpecification.Width,
                    .bottom = settings.fullscreen ? (long)screenHeight : (long)mSpecification.Height};
    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    std::string windowTitle = getWindowTitle();
    window =
        CreateWindowEx(0, mSpecification.Name.c_str(), windowTitle.c_str(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0,
                       windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hinstance, NULL);

    if (!window) {
        std::cerr << "Could not create window!\n";
        fflush(stdout);
        return nullptr;
    }

    if (!settings.fullscreen) {
        // Center on screen
        uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
        uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
        SetWindowPos(window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window);
    SetFocus(window);

    return window;
}

void Window::Destroy() {
    if (mHandle)
        glfwDestroyWindow(mHandle);
    mHandle = nullptr;
}

void Window::Update() { glfwSwapBuffers(mHandle); }

void Window::RaiseEvent(Event &event) {
    if (mSpecification.EventCallback)
        mSpecification.EventCallback(event);
}

glm::vec2 Window::GetFramebufferSize() const {
    int width, height;
    glfwGetFramebufferSize(mHandle, &width, &height);
    return {width, height};
}

glm::vec2 Window::GetMousePos() const {
    double x, y;
    glfwGetCursorPos(mHandle, &x, &y);
    return {static_cast<float>(x), static_cast<float>(y)};
}

bool Window::ShouldClose() const {
    return quitMessageReceived;
    // return glfwWindowShouldClose(mHandle) != 0;
}

} // namespace rt
