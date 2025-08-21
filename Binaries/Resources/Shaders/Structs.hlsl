#include "Common.hlsl"

/************/
/* Textures */
/************/

struct BoneTransform
{
    float4x4 transform;
};

struct VertexIn
{
	float3 Pos          : POSITION;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
	float2 TexC         : TEXCOORD;
#ifdef SKINNED
    float4 BoneWeights  : WEIGHTS;
    int4 BoneIndices    : BONEINDICES;
#endif
};

struct VertexOut
{
	float4 position         : SV_POSITION;
    float3 positionWorld    : POSITION;
    float3 normal           : NORMAL;
	float2 texUV            : TEXCOORD2;
    float4 shadowPos        : TEXCOORD1;
};

struct VertexColorIn
{
    float3 Pos          : POSITION;
    float4 Color        : COLOR;
};

struct VertexColorOut
{
    float4 Position     : SV_POSITION;
    float4 Color        : COLOR;
};

struct VertexTexIn
{
    float3 PosL : POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexTexOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexPosIn
{
    float3 Pos : POSITION;
};

struct VertexPosOut
{
    float4 Pos : SV_POSITION;
};