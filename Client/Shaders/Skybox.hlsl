#include "Common.hlsl"

struct VertexIn
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexC     : TEXCOORD;
};

struct VertexOut
{
    float4 Position         : SV_POSITION;
    float3 LocalPosition    : POSITION;
};