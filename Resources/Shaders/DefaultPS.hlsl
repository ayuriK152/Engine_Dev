#include "Structs.hlsl"
#include "LightingUtil.hlsl"

float4 PS(VertexOut pin) : SV_Target
{
    float4 totalResult;
    float3 eyePos = GetCameraPosition();
    float4 albedo = DiffuseMap.Sample(samAnisotropicWrap, pin.texUV);
    float3 eyeDir = normalize(eyePos - pin.positionWorld);
    pin.normal = normalize(pin.normal);

    Material mat = { Ambient, Diffuse, Specular, Emissive, Shiness };
    totalResult = ComputeLight(mat, albedo, pin.normal, eyeDir);

    return totalResult;
}