#include "Common.hlsl"

uint RandomSeed(uint id, float t) {
    uint h = id * 374761393u + asuint(t * 1000.0f);
    h = (h << 13) ^ h;
    return (h * (h * h * 15731u + 789221u) + 1376312589u);
}

float3 RandomDir(uint id, float t) {
    uint seed = RandomSeed(id, t);
    float x = frac(sin(seed * 12.9898f) * 43758.5453f);
    float y = frac(sin(seed * 78.233f) * 43758.5453f);
    float z = frac(sin(seed * 45.543f) * 43758.5453f);
    return normalize(float3(x * 2 - 1, y * 2 - 1, z * 2 - 1));
}

[numthreads(256, 1, 1)]
void CS(int3 threadId : SV_DispatchThreadID) {
    uint id = threadId.x;
    Particle p = particles[id];

    if (p.Age >= p.LifeTime) {
        float dir = RandomDir(id, Time);
        p.Position = EmitterPos;
        p.Velocity = ParticleInitVelocity;
        p.Age = 0.0f;
        p.LifeTime = ParticleLifeTime;
        p.TextureIndex = TextureIdx;
        p.Size = ParticleSize;
        p.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
        p.Position += p.Velocity * DeltaTime;
    }

    particles[id] = p;
}