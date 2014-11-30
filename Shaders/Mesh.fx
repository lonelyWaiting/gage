#include "Common.h"

cbuffer MaterialParams
{
	Matrix world_view_proj;
    float time;
};

Texture2D diffuse_sampler;
SamplerState diffuse_sampler_state; 

RWTexture2D<float4> g_OutBuff;

struct APP_TO_VS
{
  	float3 Pos : POSITION;
  	float3 Normal : NORMAL;
  	float2 TexCoord : TEXCOORD0;
};

struct VS_TO_PS 
{
	  float4 Pos : SV_Position;
	  float3 Normal : TEXCOORD0;
	  float2 TexCoord : TEXCOORD1;
};

void BasicVS(APP_TO_VS In, uint instance_id : SV_InstanceID, out VS_TO_PS Out)
{
    Matrix xform = world_view_proj;
    Out.Pos = mul(xform, float4(In.Pos,1));
    Out.Normal = In.Normal;
    Out.TexCoord = In.TexCoord;
}

void BasicPS(VS_TO_PS In, float4 VPos : SV_Position, out float4 OutColor : SV_Target)
{
    float lit = saturate(dot(In.Normal, float3(1,1,1)));
    lit += float3(.1, .1, .1);
	OutColor = diffuse_sampler.Sample(diffuse_sampler_state, In.TexCoord);
	OutColor *= lit;

//	OutColor = lit;

//	OutColor = float4(VPos.xy / float2(1280,1024),0,1);
}

void BasicPS_UAV(VS_TO_PS In, float4 VPos : SV_Position)
{
	g_OutBuff[VPos.xy] = float4(VPos.xy / float2(1280,1024),0,1);
}

void BasicAlphaTestPS(VS_TO_PS In, out float4 OutColor : SV_Target)
{
    float lit = saturate(dot(In.Normal, float3(1,1,1)));
	OutColor = diffuse_sampler.Sample(diffuse_sampler_state, In.TexCoord);
	OutColor *= lit;
	OutColor = float4(1,0,0,1);
//	clip(OutColor.a - 0.1);
}

struct ParticleVSToGS 
{
    float instance_id : TEXCOORD0;
};

struct ParticleGSToPS 
{
	  float4 Pos : SV_Position;
	  float3 Coord : TEXCOORD0;
	  float3 Color : TEXCOORD1;
};

void ParticleVS(out ParticleVSToGS Out, uint instance_id : SV_InstanceID, uint vertex_id : SV_VertexID)
{
    Out.instance_id = instance_id;
}

[maxvertexcount(4)]
void ParticleGS(point ParticleVSToGS input[1], inout TriangleStream<ParticleGSToPS> output)
{
    uint instance_id = input[0].instance_id;
        
    float4 viewpos = mul(view, float4(ParticleDataRO[instance_id].position,1));

    ParticleGSToPS outvertex;
    outvertex.Color = float3((ParticleDataRO[instance_id].color & 0xff000000) >> 24,
                             (ParticleDataRO[instance_id].color & 0x00ff0000) >> 16,
                             (ParticleDataRO[instance_id].color & 0x0000ff00) >>  8) / 255.0f;

    // Create a billboard around the point
    for (int i=0; i<4; i++)
    {
        float3 verts[] = 
            {
                {-1, 1,0},
                { 1, 1,0},
                {-1,-1,0},
                { 1,-1,0},
            };

        float3 vertex_pos = viewpos.xyz + verts[i] * 0.1f;

        outvertex.Pos = mul(proj,  float4(vertex_pos,1));
        outvertex.Coord = verts[i];

        output.Append(outvertex);
    }
}

void ParticlePS(ParticleGSToPS In, out float4 OutColor : SV_Target)
{
	if (length(In.Coord.xy) > 1)
		clip(-1);

	float3 normal;
	normal.x = In.Coord.x;
	normal.y = In.Coord.y;
	normal.z = sqrt(1 - normal.x*normal.x - normal.y*normal.y);
	normal = mul(normal, (float3x3)invView); // transform from view to world space

	float NdotL = saturate(dot(normalize(float3(1,1,1)), normal));
	float3 lit = In.Color * (1 + NdotL) + pow(NdotL, 16);

	OutColor = float4(lit,1);
}
