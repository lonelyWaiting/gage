#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "OBJLoader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Input.h"
#include "Camera.h"
#include "Text.h"
#include "Clock.h"
#include "GpuProfiler.h"
#include "Shader.h"
#include "System.h"
#include "Game.h"
#include <new>
// Linker input: d3d11.lib d3dx11.lib

static const char * const CLASS_NAME = "DX11 Testbed";
static const char * const WINDOW_TITLE = "DX11 Testbed";
const int SIZEX = 1280;
const int SIZEY = 720;

ID3D11Device* System::d3d_device;
ID3D11DeviceContext* System::d3d_context;
IDXGISwapChain* System::swap_chain;
ID3D11RenderTargetView* System::render_target_view;
ID3D11DepthStencilView* System::depth_stencil_view;
ID3D11UnorderedAccessView* System::render_target_uav;
uint System::framecount;

HINSTANCE g_Instance;

static ID3D11Texture2D *g_BackBuf;
static ID3D11Texture2D *g_DepthBuf;

void System::Startup(HWND hwnd)
{
    Input::Startup();
    
    //assert(XMVerifyCPUSupport());
    
    // Create device, immediate device context and swap chain
    
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    
    DXGI_SWAP_CHAIN_DESC swap_chainDesc;
    swap_chainDesc.BufferDesc.Width  = (UINT)SIZEX;
    swap_chainDesc.BufferDesc.Height = (UINT)SIZEY;
    swap_chainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swap_chainDesc.BufferDesc.RefreshRate.Denominator = 0;
//    swap_chainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    swap_chainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
    swap_chainDesc.SampleDesc.Count = 1;
    swap_chainDesc.SampleDesc.Quality = 0;
    swap_chainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS | DXGI_USAGE_SHADER_INPUT;
    swap_chainDesc.BufferCount = 2;
    swap_chainDesc.OutputWindow = hwnd;
    swap_chainDesc.Windowed = TRUE;
    swap_chainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chainDesc.Flags = 0;

	uint device_flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if _DEBUG
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        device_flags,
        featureLevels,
        _countof(featureLevels), 
        D3D11_SDK_VERSION,
        &swap_chainDesc,
        &swap_chain,
        &d3d_device,
        NULL,
        &d3d_context);
    assert(SUCCEEDED(hr));
    
    // Get swap chain's back buffer, create its render target view and set that view as render target
    
    hr = swap_chain->GetBuffer(0, __uuidof(*g_BackBuf), (void**)&g_BackBuf);
    assert(SUCCEEDED(hr));
    
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = (UINT)SIZEX;
    descDepth.Height = (UINT)SIZEY;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    
    hr = d3d_device->CreateTexture2D( &descDepth, NULL, &g_DepthBuf );
    assert(SUCCEEDED(hr));
    
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    hr = d3d_device->CreateDepthStencilView(g_DepthBuf, &descDSV, &depth_stencil_view);
    assert(SUCCEEDED(hr));
    
    hr = d3d_device->CreateRenderTargetView(g_BackBuf, NULL, &render_target_view);
    assert(SUCCEEDED(hr));


	D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
	descUAV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descUAV.Texture2D.MipSlice = 0;

	hr = d3d_device->CreateUnorderedAccessView(g_BackBuf, &descUAV, &render_target_uav);
	assert(SUCCEEDED(hr));

    // Start up subsystems
	Shader::Startup();
    TextureManager::Startup();
    MeshManager::Startup(d3d_device);
    Clock::Startup();
    Text::Startup(d3d_device);
    GpuProfiler::Startup(d3d_device);

    Game::Startup();
}

void System::Shutdown()
{
    Game::Shutdown();

    GpuProfiler::Shutdown();
    Text::Shutdown();
    Clock::Shutdown();
    MeshManager::Shutdown();
    TextureManager::Shutdown();

    Shader::Shutdown();

    SAFE_RELEASE(render_target_view);
    SAFE_RELEASE(depth_stencil_view);
    SAFE_RELEASE(g_BackBuf);
    SAFE_RELEASE(g_DepthBuf);
    SAFE_RELEASE(swap_chain);
    SAFE_RELEASE(d3d_context);
    SAFE_RELEASE(d3d_device);

    Input::Shutdown();
}

void System::GameLoop()
{
    static float cpu_frame_time;
    static Clock::Timer cpu_begin;
    cpu_begin = Clock::GetTimer();
    
    Clock::Tick();

    if (Input::key_down[' '])
        Shader::Reload();
		
    GpuProfiler::BeginFrame(d3d_context, framecount);

    d3d_context->ClearState();

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width  = (FLOAT)SIZEX;
    viewport.Height = (FLOAT)SIZEY;
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;
    d3d_context->RSSetViewports(1, &viewport);

	UINT initial_count = 0xffffffff;
	d3d_context->OMSetRenderTargets(1, &System::render_target_view, System::depth_stencil_view);
	//d3d_context->OMSetRenderTargetsAndUnorderedAccessViews(1, &System::render_target_view, System::depth_stencil_view, 1, 1, &System::render_target_uav, &initial_count);
	//d3d_context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, NULL, NULL, 1, 1, &System::render_target_uav, &initial_count);
     
    Game::GameLoop();

    Text::Print(0, 0, "CPU: %3.1f ms", cpu_frame_time * 1000.0f);
    
    Text::Draw(d3d_context);

    cpu_frame_time = Clock::GetTimeDeltaSec(cpu_begin);

	GpuProfiler::EndFrame(d3d_context, framecount);

    HRESULT hr;
    hr = swap_chain->Present(0, 0);
    assert(SUCCEEDED(hr));

    framecount++;
}
