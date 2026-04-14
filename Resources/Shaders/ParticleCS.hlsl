#include "Particle.hlsl"

uint Hash(uint n)
{
    // n = (n << 13U) ^ n;
    // return (1.0f - ((n * (n * n * 15731U + 789221U) + 1376312589U) & 0x7fffffffU) / 1073741824.0f);

    n = ((n >> 16) ^ n) * 0x45d9f3b;
    n = ((n >> 16) ^ n) * 0x45d9f3b;
    n = (n >> 16) ^ n;
    return n;
}

// 해시를 이용해 2D 또는 3D 난수 벡터를 만드는 함수
float3 RandomDir(uint instanceID, float time)
{
    uint seed = instanceID ^ Hash((uint)(time * 1000.0f));
    
    uint hX = Hash(seed);
    uint hY = Hash(seed + 192837111U); // 인접하지 않은 큰 소수를 더함
    uint hZ = Hash(seed + 374761393U);

    float3 v = float3(float(hX) / 4294967295.0f, float(hY) / 4294967295.0f, float(hZ) / 4294967295.0f) * 2.0f - 1.0f;

    if (length(v) < 0.000001f) return float3(0, 1, 0);

    // 단위 벡터로 정규화
    return normalize(v);
}

[numthreads(256, 1, 1)]
void CS(int3 threadId : SV_DispatchThreadID) {
    uint id = threadId.x;
    if (id >= 1024) return;     // TEMP - 버퍼 최대 크기 따로 받아야함

    Particle p = particles[id];

    bool isNew = false;
    uint endIdx = StartIdx + SpawnMount;

    if (id >= StartIdx && id < endIdx) isNew = true;        // 최대 크기 넘어가는건 위에서 걸러서 신경 안써도 됨
    if (endIdx >= 1024 && id < (endIdx % 1024)) isNew = true;

    // 새로 생성할 파티클인 경우
    if (isNew) {
        float3 dir = RandomDir(id, Time);
        p.Position = EmitterPos;
        p.Velocity = ParticleInitVelocity * dir;
        p.Age = 0.0f;
        p.LifeTime = ParticleLifeTime;
        p.TextureIndex = TextureIdx;
        p.Size = ParticleSize;
        p.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    // 원래 있었고 살아있는 애
    else if (p.Age < p.LifeTime) {
        p.Velocity.y += GravityFactor * DeltaTime;
        p.Position += p.Velocity * DeltaTime;
        p.Age += DeltaTime;
    }
    // 원래 있었고 죽은 애
    else {
        p.Size = float2(0.0f, 0.0f);
    }

    particles[id] = p;
}