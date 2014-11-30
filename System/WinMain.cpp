#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "OBJLoader.h"
#include "Mesh.h"
#include "Input.h"
#include "Camera.h"
#include "Text.h"
#include "Clock.h"
#include "GpuProfiler.h"
#include "System.h"
#include <new>
// Linker input: d3d11.lib d3dx11.lib

static const char * const CLASS_NAME = "DX11TEST";
static const char * const WINDOW_TITLE = "DX11 Test";
HWND g_Wnd;

static LRESULT WINAPI WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
        g_Wnd = wnd;
        System::Startup(g_Wnd);
        return 0;

    case WM_DESTROY:
        System::Shutdown();
        PostQuitMessage(0);
        return 0;

    case WM_INPUT:
        SetForegroundWindow(wnd);
        Input::OnInput(lParam);
        return 0;
    }
    
    return DefWindowProc(wnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, char* cmdLine, int showCmd)
{
    WNDCLASSEX wndClass;
    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wndClass.hbrBackground = NULL;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hInstance = instance;
    wndClass.lpfnWndProc = &WndProc;
    wndClass.lpszClassName = CLASS_NAME;

    ATOM classR = RegisterClassEx(&wndClass);
    assert(classR);

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
    DWORD exStyle = 0;

    RECT rect = { 0, 0, SIZEX, SIZEY };
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);
    g_Wnd = CreateWindowEx(
        exStyle,
        CLASS_NAME,
        WINDOW_TITLE,
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL,
        NULL,
        instance,
        0);
    assert(g_Wnd);
    
    MSG msg;
    for (;;)
    {
        Input::Tick();

        if (Input::key_down[VK_ESCAPE])
            PostMessage(g_Wnd, WM_CLOSE, 0, 0);

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return (int)msg.wParam;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        System::GameLoop();
    }
    return (int)msg.wParam;
}
