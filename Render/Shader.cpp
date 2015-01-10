#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "System.h"
#include "Shader.h"

// Info about each shader (AKA technique)
struct ShaderInfo { ShaderID id; const char* name; struct Technique* technique; };
static ShaderInfo shader_table[MAX_NUM_SHADER_IDS] = 
{
	#define SHADER(X) { X, #X, NULL },
	#include "ShaderList.h"
};

// Info about each constant buffer
static const char* constant_buffer_names[MAX_NUM_CONSTANT_BUFFER_IDS] =
{
	#define CONSTANTBUFFER(X) #X,
	#include "ShaderParameters.h"
};

ConstantBufferID GetConstantBufferID(const char* name)
{
	for (int i=0; i<MAX_NUM_CONSTANT_BUFFER_IDS; i++)
		if (strcmp(constant_buffer_names[i], name) == 0)
			return (ConstantBufferID) i;
	return (ConstantBufferID) -1;
}

// Info about each shader resource
static const char* shader_resource_names[MAX_NUM_SHADER_RESOURCE_IDS] =
{
	#define RESOURCE(X) #X,
	#include "ShaderParameters.h"
};

ShaderResourceID GetShaderResourceID(const char* name)
{
	for (int i=0; i<MAX_NUM_SHADER_RESOURCE_IDS; i++)
		if (strcmp(shader_resource_names[i], name) == 0)
			return (ShaderResourceID) i;
	return (ShaderResourceID) -1;
}

// Info about each UAV
static const char* uav_names[MAX_NUM_UAV_IDS] =
{
	#define UAV(X) #X,
	#include "ShaderParameters.h"
};

UnorderedAccessViewID GetUnorderedAccessViewID(const char* name)
{
	for (int i=0; i<MAX_NUM_UAV_IDS; i++)
		if (strcmp(uav_names[i], name) == 0)
			return (UnorderedAccessViewID) i;
	return (UnorderedAccessViewID) -1;
}

struct ShaderFiles { const char* filename; uint8* buffer; };
static ShaderFiles shader_files[] = 
{
	{ "Shaders\\ComputeShaders.fxo", NULL },
	{ "Shaders\\Mesh.fxo", NULL },
	{ "Shaders\\Text.fxo", NULL },
};
const int NumShaderFiles = sizeof(shader_files)/sizeof(shader_files[0]);

static void* constant_buffer_table[MAX_NUM_CONSTANT_BUFFER_IDS] = {0};

static ID3D11ShaderResourceView* shader_resource_table[MAX_NUM_SHADER_RESOURCE_IDS] = {0};

static ID3D11UnorderedAccessView* uav_table[MAX_NUM_UAV_IDS] = {0};

struct Sampler
{
	char                name[64];
	D3D11_SAMPLER_DESC  desc;
	uint                pad;
	ID3D11SamplerState* state;
};

struct BoundConstantBuffer
{
	uint bindslot;
	char name[64];
};

struct BoundSampler
{
	uint     bindslot;
	uint     pad;
	Sampler* sampler;
};

struct BoundShaderResource
{
	uint bindslot;
	char name[64];
};

struct BoundUAV
{
	uint bindslot;
	char name[64];
};

struct ShaderDesc
{
	uint num_constant_buffers;
	uint num_samplers;
	uint num_shader_resources;
	uint num_uavs;
	uint num_bytes_shaderblob;
	uint pad;

	BoundConstantBuffer* constant_buffers;
	BoundSampler*        samplers;
	BoundShaderResource* shader_resources;
	BoundUAV*            uav;
	void*                shaderblob;

	void* pShader;    // The D3D compiled shader. Its type is one of ID3D11Vertex/Pixel/etc/Shader.
};

struct Technique
{
	char        name[64];
	ShaderDesc* vertex_shader;	
	ShaderDesc* geometry_shader;	
	ShaderDesc* pixel_shader;	
	ShaderDesc* compute_shader;	
};

struct ShaderSet
{
	uint       num_techniques;
	uint       num_samplers;
	Technique* techniques;
	Sampler*   samplers;
};

struct FixupHeader
{
	uint num_fixups;
	uint payload_size;
	uint pad[2];

	void Fixup()
	{
		uint64* data = (uint64*) ((uint8*)this + sizeof(FixupHeader));
		uint* fixups = (uint*)   ((uint8*)this + sizeof(FixupHeader) + payload_size);

		for (uint i=0; i<num_fixups; i++)
		{
			uint offset = fixups[i];
			data[offset/sizeof(uint64)] += (uint64)data;
			offset += 0;
		}
	}
};

static void LoadShaders()
{
	for (int i=0; i<NumShaderFiles; i++)
	{
		Shader::Load(shader_files[i].filename);
	}

	for (int i=0; i<MAX_NUM_SHADER_IDS; i++)
	{
		assert(shader_table[i].technique);    // Otherwise, no technique for the ith entry in the shader list
	}
}

void Shader::Startup()
{
	LoadShaders();
}

void Shader::Reload()
{
	// Free existing shaders
	for (int i=0; i<MAX_NUM_SHADER_IDS; i++)
	{
		if (shader_table[i].technique->vertex_shader)
			((ID3D11VertexShader*)(shader_table[i].technique->vertex_shader->pShader))->Release();
		if (shader_table[i].technique->geometry_shader)
			((ID3D11GeometryShader*)(shader_table[i].technique->geometry_shader->pShader))->Release();
		if (shader_table[i].technique->pixel_shader)
			((ID3D11PixelShader*)(shader_table[i].technique->pixel_shader->pShader))->Release();
		if (shader_table[i].technique->compute_shader)
			((ID3D11ComputeShader*)(shader_table[i].technique->compute_shader->pShader))->Release();
	}

	for (int i=0; i<NumShaderFiles; i++)
	{
		ShaderSet* shader_set = (ShaderSet*) (shader_files[i].buffer + sizeof(FixupHeader));
		for (uint s=0; s<shader_set->num_samplers; s++)
		{
			shader_set->samplers[s].state->Release();
		}

		delete []shader_files[i].buffer;
	}

	LoadShaders();
}

void Shader::Shutdown()
{
}

void Shader::Load(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	uint8* buffer = new uint8[size];
	fread(buffer, 1, size, file);
	fclose(file);

	// Keep track of this allocation
	for (int i=0; i<NumShaderFiles; i++)
	{
		if (strcmp(filename, shader_files[i].filename) == 0)
		{
			shader_files[i].buffer = buffer;
		}
	}

	// Fix up and load data
	FixupHeader* fixup_header = (FixupHeader*) buffer;
	fixup_header->Fixup();

	ShaderSet* shader_set = (ShaderSet*) (buffer + sizeof(FixupHeader));

	for (uint i=0; i<shader_set->num_techniques; i++)
	{
		Technique* technique = &shader_set->techniques[i];

		if (technique->vertex_shader)
		{
			HRESULT hr = System::d3d_device->CreateVertexShader((void*)technique->vertex_shader->shaderblob, technique->vertex_shader->num_bytes_shaderblob, 0, (ID3D11VertexShader**) &technique->vertex_shader->pShader);
			assert(hr == S_OK);
		}

		if (technique->geometry_shader)
		{
			HRESULT hr = System::d3d_device->CreateGeometryShader((void*)technique->geometry_shader->shaderblob, technique->geometry_shader->num_bytes_shaderblob, 0, (ID3D11GeometryShader**) &technique->geometry_shader->pShader);
			assert(hr == S_OK);
		}

		if (technique->pixel_shader)
		{
			HRESULT hr = System::d3d_device->CreatePixelShader((void*)technique->pixel_shader->shaderblob, technique->pixel_shader->num_bytes_shaderblob, 0, (ID3D11PixelShader**) &technique->pixel_shader->pShader);
			assert(hr == S_OK);
		}

		if (technique->compute_shader)
		{
			HRESULT hr = System::d3d_device->CreateComputeShader((void*)technique->compute_shader->shaderblob, technique->compute_shader->num_bytes_shaderblob, 0, (ID3D11ComputeShader**) &technique->compute_shader->pShader);
			assert(hr == S_OK);
		}

		bool found = false;
		for (uint t=0; t<MAX_NUM_SHADER_IDS; t++)
		{
			if (strcmp(shader_table[t].name, technique->name) == 0)
			{
				assert(found == false);    // Otherwise, duplicate technique name
				shader_table[t].technique = technique;
				found = true;

				if (technique->vertex_shader)
					for (uint cb=0; cb<technique->vertex_shader->num_constant_buffers; cb++)
						assert(GetConstantBufferID(technique->vertex_shader->constant_buffers[cb].name) != -1);
				if (technique->geometry_shader)
					for (uint cb=0; cb<technique->geometry_shader->num_constant_buffers; cb++)
						assert(GetConstantBufferID(technique->geometry_shader->constant_buffers[cb].name) != -1);
				if (technique->pixel_shader)
					for (uint cb=0; cb<technique->pixel_shader->num_constant_buffers; cb++)
						assert(GetConstantBufferID(technique->pixel_shader->constant_buffers[cb].name) != -1);
				if (technique->compute_shader)
					for (uint cb=0; cb<technique->compute_shader->num_constant_buffers; cb++)
						assert(GetConstantBufferID(technique->compute_shader->constant_buffers[cb].name) != -1);

				if (technique->vertex_shader)
					for (uint cb=0; cb<technique->vertex_shader->num_shader_resources; cb++)
						assert(GetShaderResourceID(technique->vertex_shader->shader_resources[cb].name) != -1);
				if (technique->geometry_shader)
					for (uint cb=0; cb<technique->geometry_shader->num_shader_resources; cb++)
						assert(GetShaderResourceID(technique->geometry_shader->shader_resources[cb].name) != -1);
				if (technique->pixel_shader)
					for (uint cb=0; cb<technique->pixel_shader->num_shader_resources; cb++)
						assert(GetShaderResourceID(technique->pixel_shader->shader_resources[cb].name) != -1);
				if (technique->compute_shader)
					for (uint cb=0; cb<technique->compute_shader->num_shader_resources; cb++)
						assert(GetShaderResourceID(technique->compute_shader->shader_resources[cb].name) != -1);
			}
		}
		assert(found == true);    // Otherwise, technique not in the shaderlist
	}

	for (uint i=0; i<shader_set->num_samplers; i++)
	{
		HRESULT hr = System::d3d_device->CreateSamplerState(&shader_set->samplers[i].desc, &shader_set->samplers[i].state);
		assert(hr == S_OK);
	}
}

void* Shader::GetVertexShaderBlob(ShaderID id)
{
	Technique* technique = shader_table[id].technique;
	ShaderDesc* vs = technique->vertex_shader;
	return vs ? vs->shaderblob : NULL;
}

uint Shader::GetVertexShaderBlobSize(ShaderID id)
{
	Technique* technique = shader_table[id].technique;
	ShaderDesc* vs = technique->vertex_shader;
	return vs ? vs->num_bytes_shaderblob : 0;
}

void Shader::Set(ConstantBufferID id, void* data)
{
	constant_buffer_table[id] = data;
}

void Shader::Set(ShaderResourceID id, ID3D11ShaderResourceView* data)
{
	shader_resource_table[id] = data;
}

void Shader::Set(UnorderedAccessViewID id, ID3D11UnorderedAccessView* data)
{
	uav_table[id] = data;
}

void Shader::Commit(ID3D11DeviceContext* ctxt, ShaderID id)
{
	assert(shader_table[id].technique);

	Technique* technique = shader_table[id].technique;
	ShaderDesc* vs = technique->vertex_shader;
	ShaderDesc* gs = technique->geometry_shader;
	ShaderDesc* ps = technique->pixel_shader;
	ShaderDesc* cs = technique->compute_shader;

	// TODO: Track UAV state so we don't have to call this every draw call
	ID3D11UnorderedAccessView* null_uavs[D3D11_PS_CS_UAV_REGISTER_COUNT ] = {0};
	ctxt->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, null_uavs, NULL);

	// Set shaders
	ctxt->VSSetShader((ID3D11VertexShader*)  (vs ? vs->pShader : NULL), 0, 0);
	ctxt->GSSetShader((ID3D11GeometryShader*)(gs ? gs->pShader : NULL), 0, 0);
    ctxt->PSSetShader((ID3D11PixelShader*)   (ps ? ps->pShader : NULL), 0, 0);
    ctxt->CSSetShader((ID3D11ComputeShader*) (cs ? cs->pShader : NULL), 0, 0);

	// Set constant buffers
	ID3D11Buffer* constantbuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {0};

	if (vs)
	{
		for (uint i=0; i<vs->num_constant_buffers; i++)
		{
			ConstantBufferID id = GetConstantBufferID(vs->constant_buffers[i].name);
			uint bindslot = vs->constant_buffers[i].bindslot;
			constantbuffers[bindslot] = (ID3D11Buffer*) constant_buffer_table[id];
		}
		ctxt->VSSetConstantBuffers(0, vs->num_constant_buffers, constantbuffers);
	}

	if (gs)
	{
		for (uint i=0; i<gs->num_constant_buffers; i++)
		{
			ConstantBufferID id = GetConstantBufferID(gs->constant_buffers[i].name);
			uint bindslot = gs->constant_buffers[i].bindslot;
			constantbuffers[bindslot] = (ID3D11Buffer*) constant_buffer_table[id];
		}
		ctxt->GSSetConstantBuffers(0, gs->num_constant_buffers, constantbuffers);
	}

	if (ps)
	{
		for (uint i=0; i<ps->num_constant_buffers; i++)
		{
			ConstantBufferID id = GetConstantBufferID(ps->constant_buffers[i].name);
			uint bindslot = ps->constant_buffers[i].bindslot;
			constantbuffers[bindslot] = (ID3D11Buffer*) constant_buffer_table[id];
		}
		ctxt->PSSetConstantBuffers(0, ps->num_constant_buffers, constantbuffers);
	}

	if (cs)
	{
		for (uint i=0; i<cs->num_constant_buffers; i++)
		{
			ConstantBufferID id = GetConstantBufferID(cs->constant_buffers[i].name);
			uint bindslot = cs->constant_buffers[i].bindslot;
			constantbuffers[bindslot] = (ID3D11Buffer*) constant_buffer_table[id];
		}
		ctxt->CSSetConstantBuffers(0, cs->num_constant_buffers, constantbuffers);
	}

	// Set samplers
	ID3D11SamplerState* samplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {0};

	if (vs)
	{
		for (uint i=0; i<vs->num_samplers; i++)  // TODO: precompute
		{
			uint bindslot = vs->samplers[i].bindslot;
			assert(bindslot == i);
			samplers[bindslot] = vs->samplers[i].sampler->state;
		}
		ctxt->VSSetSamplers(0, vs->num_samplers, samplers);
	}

	if (gs)
	{
		for (uint i=0; i<gs->num_samplers; i++)  // TODO: precompute
		{
			uint bindslot = gs->samplers[i].bindslot;
			assert(bindslot == i);
			samplers[bindslot] = gs->samplers[i].sampler->state;
		}
		ctxt->GSSetSamplers(0, gs->num_samplers, samplers);
	}

	if (ps)
	{
		for (uint i=0; i<ps->num_samplers; i++)  // TODO: precompute
		{
			uint bindslot = ps->samplers[i].bindslot;
			assert(bindslot == i);
			samplers[bindslot] = ps->samplers[i].sampler->state;
		}
		ctxt->PSSetSamplers(0, ps->num_samplers, samplers);
	}

	if (cs)
	{
		for (uint i=0; i<cs->num_samplers; i++)  // TODO: precompute
		{
			uint bindslot = cs->samplers[i].bindslot;
			assert(bindslot == i);
			samplers[bindslot] = cs->samplers[i].sampler->state;
		}
		ctxt->CSSetSamplers(0, cs->num_samplers, samplers);
	}

	// Set shader resources
	ID3D11ShaderResourceView* shaderresources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

	if (vs)
	{
		for (uint i=0; i<vs->num_shader_resources; i++)
		{
			ShaderResourceID id = GetShaderResourceID(vs->shader_resources[i].name);  // TODO: precompute
			uint bindslot = vs->shader_resources[i].bindslot;
			assert(bindslot == i);
			shaderresources[bindslot] = (ID3D11ShaderResourceView*) shader_resource_table[id];
		}
		ctxt->VSSetShaderResources(0, vs->num_shader_resources, shaderresources);
	}

	if (gs)
	{
		for (uint i=0; i<gs->num_shader_resources; i++)
		{
			ShaderResourceID id = GetShaderResourceID(gs->shader_resources[i].name);  // TODO: precompute
			uint bindslot = gs->shader_resources[i].bindslot;
			assert(bindslot == i);
			shaderresources[bindslot] = (ID3D11ShaderResourceView*) shader_resource_table[id];
		}
		ctxt->GSSetShaderResources(0, gs->num_shader_resources, shaderresources);
	}

	if (ps)
	{
		for (uint i=0; i<ps->num_shader_resources; i++)
		{
			ShaderResourceID id = GetShaderResourceID(ps->shader_resources[i].name);  // TODO: precompute
			uint bindslot = ps->shader_resources[i].bindslot;
			assert(bindslot == i);
			shaderresources[bindslot] = (ID3D11ShaderResourceView*) shader_resource_table[id];
		}
		ctxt->PSSetShaderResources(0, ps->num_shader_resources, shaderresources);
	}

	if (cs)
	{
		for (uint i=0; i<cs->num_shader_resources; i++)
		{
			ShaderResourceID id = GetShaderResourceID(cs->shader_resources[i].name);  // TODO: precompute
			uint bindslot = cs->shader_resources[i].bindslot;
			assert(bindslot == i);
			shaderresources[bindslot] = (ID3D11ShaderResourceView*) shader_resource_table[id];
		}
		ctxt->CSSetShaderResources(0, cs->num_shader_resources, shaderresources);
	}

	// set UAVs (compute shader only)
	ID3D11UnorderedAccessView* uavs[D3D11_PS_CS_UAV_REGISTER_COUNT ];

	if (cs)
	{
		for (uint i=0; i<cs->num_uavs; i++)
		{
			UnorderedAccessViewID id = GetUnorderedAccessViewID(cs->uav[i].name);  // TODO: precompute
			uint bindslot = cs->uav[i].bindslot;
			assert(bindslot == i);
			uavs[bindslot] = (ID3D11UnorderedAccessView*) uav_table[id];
		}
		ctxt->CSSetUnorderedAccessViews(0, cs->num_uavs, uavs, NULL);
	}
}

