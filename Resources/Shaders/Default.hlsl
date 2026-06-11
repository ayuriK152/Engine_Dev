#include "Common.hlsl"

cbuffer TerrainInfo : register(b0, space1) {
    uint TerrainInstanceIdx;
    uint TerrainHeightMapIdx;
    float HeightFactor;
};

StructuredBuffer<Instance> Instances            : register(t0, space1);
StructuredBuffer<float4x4> BoneTransforms       : register(t1, space1);
StructuredBuffer<Instance> TerrainInstances     : register(t2, space1);
