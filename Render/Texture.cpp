#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Texture.h"

Array<Texture*> TextureManager::textures;

void TextureManager::Startup()
{
}

void TextureManager::Shutdown()
{
    textures.Clear();
}

Texture* TextureManager::Load(ID3D11Device* device, const char* filename)
{
    for (size_t i=0; i<textures.size; i++)
        if (_stricmp(filename, textures[i]->name)==0)
            return textures[i];

    Texture* texture = new Texture;
    texture->name = new char[strlen(filename)+1];
    strcpy(texture->name, filename);

    char* filename_with_path = new char[strlen(filename)+128];
	strcpy(filename_with_path, "assets\\");
    strcat(filename_with_path, filename);
    
    HRESULT hr = D3DX11CreateShaderResourceViewFromFile( device,
                                                         filename_with_path,
                                                         NULL, NULL,
                                                         &texture->shader_resource_view,
                                                         NULL );
	assert(SUCCEEDED(hr));

	delete filename_with_path;

    textures.Add(texture);

    return texture;
}
