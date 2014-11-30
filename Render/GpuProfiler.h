#pragma once

class GpuProfiler 
{
public:
    static void Startup(ID3D11Device*);
    static void Shutdown();

    static void BeginFrame(ID3D11DeviceContext*, uint framecount);
    static void EndFrame(ID3D11DeviceContext*, uint framecount);
};

