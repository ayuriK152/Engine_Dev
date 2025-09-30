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
StructuredBuffer<Light> Lights          : register(t0);
StructuredBuffer<Material> Materials    : register(t1);
TextureCube CubeMap                     : register(t2);
Texture2D   ShadowMap                   : register(t3);
Texture2D   DiffuseMap[100]             : register(t4);

cbuffer LightInfo : register(b0)
{
    uint gNumLights;
}

cbuffer cbPerObject : register(b1)
{
    float4x4 World;
    float4x4 WorldInv;
    uint MaterialIdx;
};

cbuffer cbCamera : register(b2)
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

cbuffer cbAnimState : register(b0, space1)
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