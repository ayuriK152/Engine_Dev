/***********/
/* Buffers */
/***********/

TextureCube CubeMap : register(t0);
Texture2D   DiffuseMap : register(t1);
Texture2D   ShadowMap : register(t2);
StructuredBuffer<float4x4> BoneTransforms: register(t0, space1);

SamplerState samPointWrap        : register(s0);
SamplerState samPointClamp       : register(s1);
SamplerState samLinearWrap       : register(s2);
SamplerState samLinearClamp      : register(s3);
SamplerState samAnisotropicWrap  : register(s4);
SamplerState samAnisotropicClamp : register(s5);

cbuffer cbPerObject : register(b1)
{
    float4x4 World;
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

/*************/
/* Functions */
/*************/

float3 GetCameraPosition()
{
    return ViewInv._41_42_43;
}