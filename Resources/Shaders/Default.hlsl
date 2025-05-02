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
	float2 texUV            : TEXCOORD;
};
