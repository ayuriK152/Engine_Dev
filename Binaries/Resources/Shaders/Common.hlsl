#include "Structs.hlsl"

/************/
/* Samplers */
/************/

SamplerState samPointWrap        : register(s0);
SamplerState samPointClamp       : register(s1);
SamplerState samLinearWrap       : register(s2);
SamplerState samLinearClamp      : register(s3);
SamplerState samAnisotropicWrap  : register(s4);
SamplerState samAnisotropicClamp : register(s5);

/***********/
/* Buffers */
/***********/

// Common
StructuredBuffer<Light> Lights  : register(t0);
TextureCube CubeMap             : register(t1);
Texture2D   ShadowMap           : register(t2);
Texture2D   DiffuseMap          : register(t3);

cbuffer LightInfo : register(b0)
{
    uint gNumLights;
}

cbuffer cbPerObject : register(b1)
{
    float4x4 World;
    float4x4 WorldInv;
};

cbuffer cbMaterial : register(b2)
{
	float4x4 MatTransform;
    float4   Ambient;
	float4   Diffuse;
    float4   Specular;
    float4   Emissive;
    float2   Tilling;
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

// Skinned Mesh
StructuredBuffer<float4x4> BoneTransforms       : register(t0, space1);
StructuredBuffer<float4x4> AnimationsTransforms : register(t1, space1);
cbuffer cbAnimState                             : register(b0, space1)
{
    float currentTick;
    uint currentAnimIdx;
    float transitionTick;
    uint nextAnimIdx;
    bool isOnTransition;
};

/*************/
/* Functions */
/*************/

float3 GetCameraPosition()
{
    return ViewInv._41_42_43;
}