#pragma once

enum ShaderID
{
    #define SHADER(X) X,
    #include "ShaderList.h"

	MAX_NUM_SHADER_IDS
};

enum ConstantBufferID
{
	#define CONSTANTBUFFER(X) X,
	#include "ShaderParameters.h"

	MAX_NUM_CONSTANT_BUFFER_IDS
};

enum ShaderResourceID
{
	#define RESOURCE(X) X,
	#include "ShaderParameters.h"

	MAX_NUM_SHADER_RESOURCE_IDS
};

enum UnorderedAccessViewID
{
	#define UAV(X) X,
	#include "ShaderParameters.h"

	MAX_NUM_UAV_IDS
};

class Shader
{
public:
    static void Startup();
    static void Shutdown();

	static void Load(const char* filename);

	static void Reload();

	static void* GetVertexShaderBlob(ShaderID id);
	static uint  GetVertexShaderBlobSize(ShaderID id);

	static void Set(ConstantBufferID, void*);
	static void Set(ShaderResourceID, ID3D11ShaderResourceView*);
	static void Set(UnorderedAccessViewID id, ID3D11UnorderedAccessView* data);

	static void Commit(ID3D11DeviceContext* pCtxt, ShaderID id);
};

