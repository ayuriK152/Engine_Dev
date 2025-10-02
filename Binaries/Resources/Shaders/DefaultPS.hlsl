#include "LightingUtil.hlsl"

float4 PS(VertexOut pin) : SV_TARGET
{
    float3 eyePos = GetCameraPosition();
    float4 albedo = DiffuseMap[Materials[pin.MaterialIdx].DiffuseMapIndex].Sample(samAnisotropicWrap, pin.texUV * Materials[pin.MaterialIdx].Tilling);
    float3 eyeDir = normalize(eyePos - pin.positionWorld);
    pin.normal = normalize(pin.normal);

    Material mat;
    mat.Ambient = Materials[pin.MaterialIdx].Ambient;
    mat.Diffuse = Materials[pin.MaterialIdx].Diffuse;
    mat.Specular = Materials[pin.MaterialIdx].Specular;
    mat.Emissive = Materials[pin.MaterialIdx].Emissive;
    mat.Shiness = Materials[pin.MaterialIdx].Shiness;
    float4 lighting = ComputeLight(mat, albedo, pin.normal, eyeDir);

    float2 shadowMapTex;
    float bias = 0.001f;
    shadowMapTex.x = pin.shadowPos.x / pin.shadowPos.w / 2.0f + 0.5f;
    shadowMapTex.y = -pin.shadowPos.y / pin.shadowPos.w / 2.0f + 0.5f;

    float depthValue = ShadowMap.Sample(samAnisotropicClamp, shadowMapTex).r;
    float lightDepthValue = pin.shadowPos.z / pin.shadowPos.w;
    lightDepthValue = lightDepthValue - bias;

    if (lightDepthValue < depthValue) {
        return lighting;
    }
    else {
        return lighting * 0.7f;
    }
}