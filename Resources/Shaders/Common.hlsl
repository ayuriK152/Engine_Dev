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

cbuffer ClientInfo : register(b0) {
    float   DeltaTime;
    float   Time;
}

cbuffer LightInfo : register(b1) {
    uint gNumLights;
}

cbuffer cbCamera : register(b2) {
    float4x4 View;
    float4x4 ViewInv;
    float4x4 Proj;
    float4x4 ProjInv;
    float4x4 ViewProj;
    float4x4 ViewProjInv;
    float2 RenderTargetSize;
    float2 InvRenderTargetSize;
};

cbuffer MeshInfo : register(b3) {
    uint InstanceStartIndex;
};

// Skinned Mesh
StructuredBuffer<float4x4> BoneTransforms       : register(t0, space1);

// Particle
RWStructuredBuffer<Particle> particles : register(u0, space2);

cbuffer cbParticleEmitterInfo : register(b0, space2) {
    float3  EmitterPos;
    float   SpawnRate;
    float   ParticleInitVelocity;
    float   ParticleLifeTime;
    float2  ParticleSize;
    uint    TextureIdx;
};

/*************/
/* Functions */
/*************/

float3 GetCameraPosition() {
    return ViewInv._41_42_43;
}

float3 GetCameraRight() {
    return float3(ViewInv._11, ViewInv._12, ViewInv._13);
}

float3 GetCameraUp() {
    return float3(ViewInv._21, ViewInv._22, ViewInv._23);
}

float3 GetCameraForward() {
    return float3(ViewInv._31, ViewInv._32, ViewInv._33);
}