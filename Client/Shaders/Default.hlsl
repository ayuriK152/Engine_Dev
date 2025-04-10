//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include "LightingUtil.hlsl"

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

/***********/
/* Buffers */
/***********/

Texture2D    DiffuseMap : register(t0);
StructuredBuffer<float4x4> BoneTransforms: register(t1);

SamplerState samPointWrap        : register(s0);
SamplerState samPointClamp       : register(s1);
SamplerState samLinearWrap       : register(s2);
SamplerState samLinearClamp      : register(s3);
SamplerState samAnisotropicWrap  : register(s4);
SamplerState samAnisotropicClamp : register(s5);

cbuffer cbPerObject : register(b1)
{
    float4x4 World;
	float4x4 TexTransform;
};

cbuffer cbMaterial : register(b2)
{
	float4x4 MatTransform;
    float4   Ambient;
	float4   Diffuse;
    float4   Specular;
    float4   Emissive;
    float    Shiness;
};

cbuffer cbCamera : register(b3)
{
    float4x4 View;
    float4x4 ViewInv;
    float4x4 Proj;
    float4x4 ProjInv;
    float4x4 ViewProj;
    float4x4 ViewProjInv;
    float2 RenderTargetSize;
    float2 InvRenderTargetSize;
};

/*************/
/* Functions */
/*************/

float3 GetCameraPosition()
{
    return ViewInv._41_42_43;
}
