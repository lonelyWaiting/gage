#include "Shared.h"

cbuffer PerFrameParameters
{
    Matrix view;
	Matrix invView;
    Matrix proj;
    Matrix worldViewProj;
    float  dt;
};

struct Particle
{
    float3 position;
    float3 velocity;
    uint   color;
};

StructuredBuffer<Particle> ParticleDataRO;

RWStructuredBuffer<Particle> ParticleDataRW;

cbuffer CollisionPlanes
{
	float4 collision_plane[NUM_COLLISION_PLANES];
};
