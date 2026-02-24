#include "Particle.hlsl"

float4 PS(GS_OUT input) : SV_Target {
    Texture2D tex = TextureMaps[input.texIndex];
    float4 texColor = tex.Sample(samAnisotropicWrap, input.uv);

    // 파티클 자체 색상과 텍스처 색상 곱
    return texColor * input.color;
}