#include "Common.hlsl"

VertexPosColorOut VS(VertexPosColorIn vin) {
    VertexPosColorOut vout;

    vout.Position = mul(float4(vin.Pos, 1.0f), ViewProj);
    vout.Color = vin.Color;

    return vout;
}