#pragma once

#include <Array.h>

struct Texture
{
    char* name;
    ID3D11ShaderResourceView* shader_resource_view;
};

struct TextureManager
{
    static void Startup();
    static void Shutdown();
    static Texture* Load(ID3D11Device* device, const char* filename);

    static Array<Texture*> textures;
};
