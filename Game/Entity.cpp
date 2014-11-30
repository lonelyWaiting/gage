#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Shader.h"
#include "Entity.h"
#include "Mesh.h"
#include "OBJLoader.h"
#include "Texture.h"

static ID3D11Buffer* material_constant_buffer;

struct MaterialCB
{
	XMMATRIX world_view_proj;
    float time;
};

Entity::Entity(ID3D11Device* device, const char* model_name)
{
    mesh = MeshManager::LoadOBJ(device, model_name);

    // Create constant buffer
    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.ByteWidth = (uint) align16(sizeof(MaterialCB));
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bufDesc, NULL, &material_constant_buffer);
    assert(SUCCEEDED(hr));
}

Entity::~Entity()
{
    SAFE_RELEASE(mesh->vertex_buffer);
    for (uint i=0; i<mesh->subsets.size; i++)
        SAFE_RELEASE(mesh->subsets[i]->index_buffer);
	SAFE_RELEASE(material_constant_buffer);
}

void Entity::Update()
{
}

void Entity::Render(ID3D11DeviceContext* ctxt, float time, const XMMATRIX& view, const XMMATRIX& proj)
{
    // Set constant buffer
    MaterialCB cb;
    cb.time = time;

	// We only have one entity at the moment, and it is at 0,0,0
    XMMATRIX world = XMMatrixRotationRollPitchYaw(0, 0, 0);
	XMVECTOR pos = {0, 0, 0, 1};
    world.r[3] = pos;
        
	cb.world_view_proj = world * view * proj;
	cb.time = 0;
    
    ctxt->UpdateSubresource(material_constant_buffer, 0, NULL, &cb, 0, 0);
    
	Shader::Set(MaterialParams, material_constant_buffer);

    // Set up for drawing the subsets
    UINT vbStride = sizeof(Vertex), vbOffset = 0;
    ctxt->IASetVertexBuffers(0, 1, &mesh->vertex_buffer, &vbStride, &vbOffset);
    ctxt->IASetInputLayout(MeshManager::input_layout);
    ctxt->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Draw subsets
    for (uint i=0; i<mesh->subsets.size; i++)
    {
		if (mesh->subsets[i]->diffuse_texture)
			Shader::Set(diffuse_sampler, mesh->subsets[i]->diffuse_texture->shader_resource_view);
		else
			Shader::Set(diffuse_sampler, NULL);
        Shader::Commit(ctxt, ShaderRender);

        ctxt->IASetIndexBuffer(mesh->subsets[i]->index_buffer, DXGI_FORMAT_R32_UINT, 0);
        ctxt->DrawIndexed(mesh->subsets[i]->num_triangles*3, 0, 0);
    }
}
