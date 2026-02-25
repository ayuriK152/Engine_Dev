#include "Lighting.hlsl"

float4 PS(VertexOut pin) : SV_TARGET {
    Material mat = Materials[pin.MaterialIdx];

    float3 eyePos = GetCameraPosition();
    float4 albedo = TextureMaps[mat.DiffuseMapIndex].Sample(samAnisotropicWrap, pin.TexUV * mat.Tilling);
    float3 eyeDir = normalize(eyePos - pin.PositionWorld);
    pin.Normal = normalize(pin.Normal);

    if (mat.NormalMapIndex != 0) {
        float3 normalMapSample = TextureMaps[mat.NormalMapIndex].Sample(samAnisotropicWrap, pin.TexUV * mat.Tilling).rgb;
        float3 normalT = 2.0f * normalMapSample - 1.0f;

        float3 N = pin.Normal;
        float3 T = normalize(pin.Tangent - dot(pin.Tangent, N) * N);
        float3 B = cross(N, T);

        float3x3 TBN = float3x3(T, B, N);
        pin.Normal = mul(normalT, TBN);
    }

    float4 lighting = ComputeLight(mat, albedo, pin.Normal, eyeDir);

    float2 shadowMapTex;
    float bias = 0.001f;
    shadowMapTex.x = pin.ShadowPos.x / pin.ShadowPos.w / 2.0f + 0.5f;
    shadowMapTex.y = -pin.ShadowPos.y / pin.ShadowPos.w / 2.0f + 0.5f;

    float depthValue = ShadowMap.Sample(samAnisotropicClamp, shadowMapTex).r;
    float lightDepthValue = pin.ShadowPos.z / pin.ShadowPos.w;
    lightDepthValue = lightDepthValue - bias;

    if (lightDepthValue < depthValue) {
        return lighting;
    }
    else {
        return lighting * 0.7f;
    }
}