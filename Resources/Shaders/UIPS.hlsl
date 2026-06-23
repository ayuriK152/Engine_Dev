#include "UI.hlsl"

float4 PS(UIVertexOut input) : SV_Target {
    return TextureMaps[input.TextureIdx].Sample(samLinearClamp, input.UV) * input.Color * (1.0 - CameraColorBlend.a) + CameraColorBlend * CameraColorBlend.a;
}