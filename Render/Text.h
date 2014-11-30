#pragma once

class Text
{
public:
    static void Startup(ID3D11Device* device);
    static void Shutdown();

    // XY is in screenspace (eg, 0,0 .. 1280,720)
    // The string can be any valid printf string, and is buffered internally
    static void Print(float x, float y, const char* string, ...);

    // Issue D3D draw calls to render the text, and clear the internal buffer
    static void Draw(ID3D11DeviceContext* ctxt);
};
