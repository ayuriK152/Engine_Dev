#include "Common.hlsl"

float4 PS(VertexTexOut pin) : SV_Target
{
    float depth = ShadowMap.Sample(samAnisotropicClamp, pin.TexC).r;
    return float4(depth, depth, depth, 1);
}