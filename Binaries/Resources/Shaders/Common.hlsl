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
StructuredBuffer<Instance> Instances    : register(t0);
StructuredBuffer<Material> Materials    : register(t1);
StructuredBuffer<Light> Lights          : register(t2);
TextureCube CubeMap                     : register(t3);
Texture2D   ShadowMap                   : register(t4);
Texture2D   DiffuseMap[100]             : register(t5);

cbuffer LightInfo : register(b0) {
    uint gNumLights;
}

cbuffer cbCamera : register(b1) {
    float4x4 View;
    float4x4 ViewInv;
    float4x4 Proj;
    float4x4 ProjInv;
    float4x4 ViewProj;
    float4x4 ViewProjInv;
    float2 RenderTargetSize;
    float2 InvRenderTargetSize;
};

cbuffer MeshInfo : register(b2) {
    uint InstanceStartIndex;
};

// Skinned Mesh
StructuredBuffer<float4x4> BoneTransforms       : register(t0, space1);
StructuredBuffer<float4x4> AnimationsTransforms : register(t1, space1);

cbuffer cbAnimState : register(b0, space1) {
    float CurrentTick;
    uint CurrentAnimIdx;
    float TransitionTick;
    uint NextAnimIdx;
    bool IsOnTransition;
};

/*************/
/* Functions */
/*************/

float3 GetCameraPosition() {
    return ViewInv._41_42_43;
}