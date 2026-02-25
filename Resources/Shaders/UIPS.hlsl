#include "UI.hlsl"

float4 PS(UIVertexOut input) : SV_Target {
    return TextureMaps[input.TextureIdx].Sample(samLinearClamp, input.UV) * input.Color;
}