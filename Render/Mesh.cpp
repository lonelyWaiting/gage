#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJLoader.h"
#include "Texture.h"

ID3D11InputLayout* MeshManager::input_layout;
Array<Mesh*> MeshManager::meshes;

void MeshManager::Startup(ID3D11Device* device)
{
    // Create input layout
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
	HRESULT hr = device->CreateInputLayout(inputDesc,
                                           _countof(inputDesc),
                                           Shader::GetVertexShaderBlob(ShaderRender),
                                           Shader::GetVertexShaderBlobSize(ShaderRender),
                                           &input_layout);
    assert(SUCCEEDED(hr));
}

void MeshManager::Shutdown()
{
    SAFE_RELEASE(input_layout);
}

Mesh* MeshManager::LoadOBJ(ID3D11Device* device, const char* filename)
{
    for (size_t i=0; i<meshes.size; i++)
        if (_stricmp(filename, meshes[i]->name)==0)
            return meshes[i];
    
    OBJLoader meshloader;
    meshloader.Load(filename);

    Mesh* mesh = new Mesh;
    meshes.Add(mesh);
    mesh->name = new char[strlen(filename)+1 + 128];
    strcpy(mesh->name, filename);
    
    // Create and initialize vertex buffer
    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory(&bufDesc, sizeof(bufDesc));
	bufDesc.ByteWidth = sizeof(Vertex) * (int)meshloader.num_vertices;
    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA bufData;
    ZeroMemory(&bufData, sizeof(bufData));
	bufData.pSysMem = meshloader.vertices;

    HRESULT hr = device->CreateBuffer(&bufDesc, &bufData, &mesh->vertex_buffer);
    assert(SUCCEEDED(hr));

    // Create subsets
    for (uint i=0; i<(uint)meshloader.subsets.size; i++)
    {
        Mesh::Subset* subset = new Mesh::Subset;
        mesh->subsets.Add(subset);

        if (meshloader.subsets[i]->material->map_Kd[0])
            subset->diffuse_texture = TextureManager::Load(device, meshloader.subsets[i]->material->map_Kd);
        else
            subset->diffuse_texture = NULL;
        
        ZeroMemory(&bufDesc, sizeof(bufDesc));
        bufDesc.ByteWidth = sizeof(uint) * (int)meshloader.subsets[i]->num_triangles*3;
        bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        ZeroMemory(&bufData, sizeof(bufData));
        bufData.pSysMem = meshloader.subsets[i]->triangles;

        hr = device->CreateBuffer(&bufDesc, &bufData, &subset->index_buffer);
        assert(SUCCEEDED(hr));

        subset->num_triangles = meshloader.subsets[i]->num_triangles;
    }

    return mesh;
}
