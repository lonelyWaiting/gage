#pragma once

class System
{
public:
    static void Startup(HWND hwnd);
    static void Shutdown();

    static void GameLoop();

    static ID3D11Device*			  d3d_device;
    static ID3D11DeviceContext*		  d3d_context;
    static IDXGISwapChain*			  swap_chain;

    static ID3D11RenderTargetView*	  render_target_view;
	static ID3D11UnorderedAccessView* render_target_uav;
    static ID3D11DepthStencilView*	  depth_stencil_view;

    static uint framecount;
};
