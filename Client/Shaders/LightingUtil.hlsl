#define MaxLights 16

struct Light
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emmissive;
    float3 Direction;
    int LightType;
    float2 FalloffInfo;
    float SpotPower;
    int padding1;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emmissive;
};
