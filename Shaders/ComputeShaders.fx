#include "Common.h"

groupshared uint test[8*1024];    // groupshared has max 32K in D3D11

struct Derivative 
{
	float3 dx;
	float3 dv;
};

float3 acceleration(const Particle state)
{
    return float3(0, -9.8f, 0);
}

Derivative evaluate(const Particle initial, const float dt, const Derivative d)
{
	Particle state = initial;
    state.position = initial.position + d.dx*dt;
    state.velocity = initial.velocity + d.dv*dt;

    Derivative output;
    output.dx = state.velocity;
    output.dv = acceleration(state);
    return output;
}

void integrate_rk4(inout Particle state, float dt)
{
    Derivative start;
    start.dx = 0;
    start.dv = 0;
 
	Derivative a = evaluate(state, 0.0f, start);
    Derivative b = evaluate(state, dt*0.5f, a);
    Derivative c = evaluate(state, dt*0.5f, b);
    Derivative d = evaluate(state, dt, c);
    
    const float3 dxdt = 1.0f/6.0f * (a.dx + 2.0f*(b.dx + c.dx) + d.dx);
    const float3 dvdt = 1.0f/6.0f * (a.dv + 2.0f*(b.dv + c.dv) + d.dv);
    
    state.position += dxdt * dt;
    state.velocity += dvdt * dt;
}

void integrate_euler(inout Particle state, float dt)
{
    state.velocity += acceleration(state) * dt;
    state.position += state.velocity * dt;
}

[numthreads( 1024, 1, 1 )]
void ParticleUpdateCS( uint3 threadIDInGroup : SV_GroupThreadID, uint3 groupID : SV_GroupID )
{
	int num_steps = 1;
	for (int i=0; i<num_steps; i++)
	{
		float sub_dt = dt/(float)num_steps;
	    Particle p = ParticleDataRW[threadIDInGroup.x + groupID.x*1024];
		float3 original_position = p.position;
        float3 original_velocity = p.velocity;

        integrate_rk4(p, sub_dt);

		//p.color = 0xff0000ff;
		for (int i=0; i<NUM_COLLISION_PLANES; i++)
		{
			float  d = dot( float4(p.position,1), collision_plane[i] );
			float3 n = collision_plane[i].xyz;
			if ( d < 0 )
			{
                p.position = original_position;
				p.velocity = 0.85f * reflect(p.velocity, n);
				//p.color = 0xffffffff;
			}
		}

		test[groupID.x] = 0;
		GroupMemoryBarrierWithGroupSync();

		ParticleDataRW[threadIDInGroup.x + groupID.x*1024] = p;

		// InterlockedAdd(ParticleDataRW[threadIDInGroup.x + groupID.x*1024].color, 0x01010101);
	}
}
