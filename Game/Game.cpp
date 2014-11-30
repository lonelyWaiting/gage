#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Entity.h"
#include "Particles.h"
#include "Input.h"
#include "Camera.h"
#include "Text.h"
#include "Clock.h"
#include "GpuProfiler.h"
#include "System.h"
#include "Game.h"
#include "Shader.h"
#include "Script.h"
#include <new>

Camera*    camera;
Entity*    entity;
Particles* particles;

struct PerFrameParametersCB
{
    XMMATRIX view;
	XMMATRIX inv_view;
    XMMATRIX proj;
    XMMATRIX view_proj;
    float    dt;
};

static ID3D11Buffer *perframeparameters_constant_buffer;

static void ResetCameraPosition(Camera* pCamera)
{
	// pCamera->MoveTo(XMVectorSet(0,50,-150,1));
	pCamera->MoveTo(XMVectorSet(220.626694f, 55.037178f, -6.500339f, 1), -3.14159f/2);
}

void Game::Startup()
{
	//pFoliageObject = new FoliageObject(System::d3d_device, "Assets\\mobster.obj", pTerrain);
	//pFoliageObject = new FoliageObject(System::d3d_device, "Assets\\teapot.obj", pTerrain);
    //pFoliageObject = new FoliageObject(System::d3d_device, "Assets\\cube.obj", pTerrain);
    //pFoliageObject = new FoliageObject(System::d3d_device, "Assets\\pine_tree.obj", pTerrain);
    //pFoliageObject = new FoliageObject(System::d3d_device, "Assets\\dragon.obj", pTerrain);
    entity = new Entity(System::d3d_device, "Assets\\sponza.obj");

    void* camera_object = _aligned_malloc(sizeof(Camera), 16);
    camera = new (camera_object) Camera();
	ResetCameraPosition(camera);

	particles = new Particles(System::d3d_device, System::d3d_context);

    // Create constant buffers
    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.ByteWidth = (uint) align16(sizeof(PerFrameParametersCB));
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.CPUAccessFlags = 0;
	HRESULT hr = System::d3d_device->CreateBuffer(&bufDesc, NULL, &perframeparameters_constant_buffer);
    assert(SUCCEEDED(hr));

	ScriptStartup("Scripts/script.lua");
}   

void Game::Shutdown()
{
	ScriptShutdown();

    delete entity;
	delete particles;

    _aligned_free(camera);

	SAFE_RELEASE(perframeparameters_constant_buffer);
}

void Game::GameLoop()
{
    // Get D3D context
    ID3D11DeviceContext* pCtxt = System::d3d_context;

    // Get system time
	static float last_time = 0;
    float time = Clock::GetAbsTimeSec();
	dpf("%f\n", time);
	float dt = time - last_time;
	dt = min(dt, 0.1f);
	last_time = time;

	ScriptUpdate(dt);

	particles->Update();

	// Set per frame parameters
    XMVECTOR det;
	PerFrameParametersCB frame;
	frame.view = camera->view;
	frame.inv_view = XMMatrixInverse(&det, camera->view);
	frame.proj = camera->proj;
    frame.view_proj = frame.view * frame.proj;
	frame.dt = dt;
	pCtxt->UpdateSubresource(perframeparameters_constant_buffer, 0, NULL, &frame, 0, 0);
	Shader::Set(PerFrameParameters, perframeparameters_constant_buffer);

    // Update camera
    camera->Update();

	if (Input::gamepad_left_pop)
	{
		ResetCameraPosition(camera);
	}
	
    // Begin rendering
    float clearColor[4] = { 0.4f, 0.6f, 1.0f, 1.f };
    pCtxt->ClearRenderTargetView(System::render_target_view, clearColor);
    pCtxt->ClearDepthStencilView(System::depth_stencil_view, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    //pTerrain->Render(pCtxt, time, pCamera->view, pCamera->proj);

	entity->Render(pCtxt, time, camera->view, camera->proj);

	particles->Render(pCtxt, time, camera->view, camera->proj);

//	pComputeShader->Render(pCtxt, time, pCamera->view, pCamera->proj);
}
