#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Shader.h"
#include "System.h"
#include "Particles.h"
#include "Shaders/Shared.h"
#include "Input.h"
#include "Clock.h"

int num_particles = 256*1024;


static ID3D11InputLayout *input_layout;
static ID3D11Buffer *particle_data_buffer;
static ID3D11ShaderResourceView *particle_data_srv;
static ID3D11UnorderedAccessView *particle_data_uav;
static ID3D11Buffer *collisionplanes_constant_buffer;

struct Particle
{
	float x,y,z;
	float vx, vy, vz;
	uint color;
};

struct CollisionPlanesCB
{
    XMVECTOR collision_planes[NUM_COLLISION_PLANES];
};

static void ResetParticles(Particle* p)
{
	srand(0);
	for (int i=0; i<num_particles; i++)
	{
		uint colors[] = {
			0x60000000,
			0x00400000,
			0x00008000
		};
		p[i].x = (rand() / (float)RAND_MAX * 2 - 1) * 5; 
		p[i].y = 75 + (rand() / (float)RAND_MAX * 2 - 1) * 10;
		p[i].z = (rand() / (float)RAND_MAX * 2 - 1) * 5; 
		p[i].vx = (rand() / (float)RAND_MAX * 2 - 1) * 10.5f;
		p[i].vy = (rand() / (float)RAND_MAX * 2 - 1) * 10.5f;
		p[i].vz = (rand() / (float)RAND_MAX * 2 - 1) * 10.5f;
		p[i].color = colors[rand() % 3];
	}
}

Particles::Particles(ID3D11Device* pDev, ID3D11DeviceContext* pCtxt)
{
    // Create input layout. The vertex shader implicitly creates its data, but we still need an input layout, so
	// I just make one with a single attribute.
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
	HRESULT hr = pDev->CreateInputLayout(inputDesc, _countof(inputDesc), Shader::GetVertexShaderBlob(ShaderParticles), Shader::GetVertexShaderBlobSize(ShaderParticles), &input_layout);
    assert(SUCCEEDED(hr));

    // Create buffer for particle state
    D3D11_BUFFER_DESC particle_buffer_desc;
    ZeroMemory(&particle_buffer_desc, sizeof(particle_buffer_desc));
    particle_buffer_desc.ByteWidth = sizeof(Particle)*num_particles;
    particle_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    particle_buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	particle_buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    particle_buffer_desc.CPUAccessFlags = 0;
	particle_buffer_desc.StructureByteStride = sizeof(Particle);

	Particle* p = new Particle[num_particles];
	ResetParticles(p);

	D3D11_SUBRESOURCE_DATA particle_data;
    ZeroMemory(&particle_data, sizeof(particle_data));
	particle_data.pSysMem = p;

	hr = pDev->CreateBuffer(&particle_buffer_desc, &particle_data, &particle_data_buffer);
    assert(SUCCEEDED(hr));

	delete []p;

    D3D11_SHADER_RESOURCE_VIEW_DESC particle_data_srv_desc;
    particle_data_srv_desc.Format = DXGI_FORMAT_UNKNOWN;
    particle_data_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    particle_data_srv_desc.Buffer.ElementOffset = 0;
    particle_data_srv_desc.Buffer.ElementWidth = num_particles;
    hr = pDev->CreateShaderResourceView(particle_data_buffer, &particle_data_srv_desc, &particle_data_srv);
    assert(SUCCEEDED(hr));

    D3D11_UNORDERED_ACCESS_VIEW_DESC particle_data_uav_desc;
    particle_data_uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    particle_data_uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    particle_data_uav_desc.Buffer.FirstElement = 0;
    particle_data_uav_desc.Buffer.NumElements = num_particles;
	particle_data_uav_desc.Buffer.Flags = 0;
    hr = pDev->CreateUnorderedAccessView(particle_data_buffer, &particle_data_uav_desc, &particle_data_uav);
    assert(SUCCEEDED(hr));

    // Create constant buffers
    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.ByteWidth = (uint) align16(sizeof(CollisionPlanesCB));
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.CPUAccessFlags = 0;

	hr = pDev->CreateBuffer(&bufDesc, NULL, &collisionplanes_constant_buffer);
    assert(SUCCEEDED(hr));

	time_since_last_restart = 0;
}

Particles::~Particles()
{
}

void Particles::Update()
{
	time_since_last_restart += Clock::GetDeltaTimeSec();
}

void Particles::Render(ID3D11DeviceContext* pCtxt, float time, const XMMATRIX& view, const XMMATRIX& proj)
{
	if (Input::gamepad_left_pop || time_since_last_restart > 15.0f)
	{
		Particle* p = new Particle[num_particles];
		ResetParticles(p);
		pCtxt->UpdateSubresource(particle_data_buffer, 0, NULL, p, 0, 0);
		delete p;
		time_since_last_restart = 0;
	}

	CollisionPlanesCB collision_planes;
	collision_planes.collision_planes[0] = XMVectorSet( 1,  0,  0, 15);
	collision_planes.collision_planes[1] = XMVectorSet(-1,  0,  0, 15);
	collision_planes.collision_planes[2] = XMVectorSet( 0,  0,  1, 50);
	collision_planes.collision_planes[3] = XMVectorSet( 0,  0, -1, 50);
	collision_planes.collision_planes[4] = XMVectorSet( 0,  1,  0, -50);
	pCtxt->UpdateSubresource(collisionplanes_constant_buffer, 0, NULL, &collision_planes, 0, 0);

	Shader::Set(ParticleDataRO, particle_data_srv);
	Shader::Set(ParticleDataRW, particle_data_uav);
	Shader::Set(CollisionPlanes, collisionplanes_constant_buffer);

	// Update
	Shader::Commit(pCtxt, ParticleUpdate);
	pCtxt->Dispatch(num_particles/1024, 1, 1);

	// Render
	Shader::Commit(pCtxt, ShaderParticles);
	
	ID3D11Buffer* pNullBuffer = NULL;
	UINT stride = 0;
	UINT offset = 0;

    pCtxt->IASetVertexBuffers(0, 1, &pNullBuffer, &stride, &offset);
    pCtxt->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
    pCtxt->IASetInputLayout(input_layout);
    pCtxt->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    pCtxt->DrawInstanced(1, num_particles, 0, 0);
}
