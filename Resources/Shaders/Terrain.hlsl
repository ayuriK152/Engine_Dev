#include "Common.hlsl"

cbuffer TerrainInfo : register(b0, space1) {
    uint TerrainInstanceIdx;
    uint TerrainHeightMapIdx;
    float HeightFactor;
};

StructuredBuffer<Instance> TerrainInstances     : register(t0, space1);