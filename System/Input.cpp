#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "core.h"
#include "Input.h"
#include "XInput.h"
#pragma comment(lib, "xinput.lib")

// The best resource for Windows' RawInput library is here:
// http://www.toymaker.info/Games/html/raw_input.html

char  Input::key_down[255] = {0};
bool  Input::mousebutton_left_down = false;
bool  Input::mousebutton_middle_down = false;
bool  Input::mousebutton_right_down = false;
float Input::mouse_dx = 0;
float Input::mouse_dy = 0;
float Input::gamepad_left_x;
float Input::gamepad_left_y;
float Input::gamepad_right_x;
float Input::gamepad_right_y;
float Input::gamepad_left_trigger;
float Input::gamepad_right_trigger;
bool  Input::gamepad_button_a;
bool  Input::gamepad_button_b;
bool  Input::gamepad_button_x;
bool  Input::gamepad_button_y;
bool  Input::gamepad_left_shoulder;
bool  Input::gamepad_right_shoulder;
bool  Input::gamepad_left_pop;
bool  Input::gamepad_right_pop;

void Input::Startup()
{
    RAWINPUTDEVICE Rid[2];
    
    Rid[0].usUsagePage = 0x01; 
    Rid[0].usUsage = 0x02; 
    Rid[0].dwFlags = 0;
    Rid[0].hwndTarget = 0;
    
    Rid[1].usUsagePage = 0x01; 
    Rid[1].usUsage = 0x06; 
    Rid[1].dwFlags = 0;
    Rid[1].hwndTarget = 0;
    
    if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) 
    {
        // raw input registration failed. Call GetLastError for the cause of the error
        assert(0);
    }

	XInputEnable(true);
}

void Input::Shutdown()
{
}

void Input::OnInput(LPARAM lParam)
{
	// Keyboard/mouse input
    const uint buffersize = 64;
    static BYTE lpb[buffersize];

    UINT dwSize;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    assert(dwSize <= buffersize);

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
        dpf("GetRawInputData does not return correct size !\n"); 
    
    RAWINPUT* raw = (RAWINPUT*)lpb;

    if (raw->header.dwType == RIM_TYPEKEYBOARD) 
    {
        uint key_code = raw->data.keyboard.VKey;
        bool key_up = raw->data.keyboard.Flags & RI_KEY_BREAK;

        key_down[key_code] = !key_up;
    } else if (raw->header.dwType == RIM_TYPEMOUSE) {
        if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
            mousebutton_left_down = true;
        if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
            mousebutton_left_down = false;
        if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
            mousebutton_middle_down = true;
        if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
            mousebutton_middle_down = false;
        if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
            mousebutton_right_down = true;
        if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
            mousebutton_right_down = false;

        mouse_dx = (float)raw->data.mouse.lLastX;
        mouse_dy = (float)raw->data.mouse.lLastY;
    }
}

void Input::Tick()
{
    mouse_dx = 0;
    mouse_dy = 0;

	// Gamepad input
	XINPUT_STATE state;
	ZeroMemory( &state, sizeof(XINPUT_STATE) );

	// Assume controller 0
	int gamepad_index = 0;
    DWORD result = XInputGetState( gamepad_index, &state );

    if( result != ERROR_SUCCESS )
	{
		gamepad_left_x = 0;
		gamepad_left_y = 0;
		gamepad_right_x = 0;
		gamepad_right_y = 0;
		gamepad_left_trigger = 0;
		gamepad_right_trigger = 0;
		gamepad_button_a = false;
		gamepad_button_b = false;
		gamepad_button_x = false;
		gamepad_button_y = false;
	    gamepad_left_shoulder = false;
		gamepad_right_shoulder = false;
	    gamepad_left_pop = false;
		gamepad_right_pop = false;
		return;
	}

	gamepad_button_a = (state.Gamepad.wButtons & 0x1000) ? true : false;
	gamepad_button_b = (state.Gamepad.wButtons & 0x2000) ? true : false;
	gamepad_button_x = (state.Gamepad.wButtons & 0x4000) ? true : false;
	gamepad_button_y = (state.Gamepad.wButtons & 0x8000) ? true : false;
    gamepad_left_shoulder = (state.Gamepad.wButtons & 0x0100) ? true : false;
    gamepad_right_shoulder = (state.Gamepad.wButtons & 0x0200) ? true : false;
    gamepad_left_pop = (state.Gamepad.wButtons & 0x0040) ? true : false;
    gamepad_right_pop = (state.Gamepad.wButtons & 0x0080) ? true : false;

	gamepad_left_x = state.Gamepad.sThumbLX / 32767.f; 
	gamepad_left_y = state.Gamepad.sThumbLY / 32767.f;
	gamepad_right_x = state.Gamepad.sThumbRX / 32767.f; 
	gamepad_right_y = state.Gamepad.sThumbRY / 32767.f;
    gamepad_left_trigger = state.Gamepad.bLeftTrigger / 255.f;
    gamepad_right_trigger = state.Gamepad.bRightTrigger / 255.f;

	float deadzone = 0.1f;

	float left_magnitude  = sqrtf(gamepad_left_x*gamepad_left_x + gamepad_left_y*gamepad_left_y);
	if (left_magnitude < deadzone)
	{
		gamepad_left_x = 0;
		gamepad_left_y = 0;
	}

	float right_magnitude = sqrtf(gamepad_right_x*gamepad_right_x + gamepad_right_y*gamepad_right_y);
	if (right_magnitude < deadzone)
	{
		gamepad_right_x = 0;
		gamepad_right_y = 0;
	}

    if (gamepad_left_trigger < deadzone)
        gamepad_left_trigger = 0;

    if (gamepad_right_trigger < deadzone)
        gamepad_right_trigger = 0;
}
