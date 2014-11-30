#ifndef CONSTANTBUFFER
#define CONSTANTBUFFER(X)
#endif

#ifndef RESOURCE
#define RESOURCE(X)
#endif

#ifndef UAV
#define UAV(X)
#endif

CONSTANTBUFFER(MaterialParams)
CONSTANTBUFFER(PerFrameParameters)
CONSTANTBUFFER(CollisionPlanes)

RESOURCE(font_sampler)
RESOURCE(diffuse_sampler)
RESOURCE(heightmap_sampler)
RESOURCE(tex0_sampler)
RESOURCE(tex1_sampler)
RESOURCE(tex0_normal_sampler)
RESOURCE(tex1_normal_sampler)
RESOURCE(ParticleDataRO)

UAV(ParticleDataRW)

#undef CONSTANTBUFFER
#undef RESOURCE
#undef UAV