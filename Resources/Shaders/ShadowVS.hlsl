#include "Structs.hlsl"
#include "LightingUtil.hlsl"

VertexPosOut VS(VertexPosIn vin)
{
    VertexPosOut vout;

    float4 posW = mul(float4(vin.Pos, 1.0f), World);
    vout.Pos = mul(posW, mul(Lights[0].View, Lights[0].Proj));

    return vout;
}