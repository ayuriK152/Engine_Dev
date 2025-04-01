//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include "LightingUtil.hlsl"

struct BoneTransform
{
    float4x4 transform;
};

Texture2D    DiffuseMap : register(t0);
StructuredBuffer<float4x4> BoneTransforms: register(t1);

SamplerState samPointWrap        : register(s0);
SamplerState samPointClamp       : register(s1);
SamplerState samLinearWrap       : register(s2);
SamplerState samLinearClamp      : register(s3);
SamplerState samAnisotropicWrap  : register(s4);
SamplerState samAnisotropicClamp : register(s5);

// Consider about delete this one
cbuffer cbPerObject : register(b0)
{
    float4x4 World;
	float4x4 TexTransform;
};

cbuffer cbMaterial : register(b1)
{
	float4x4 MatTransform;
    float4   Ambient;
	float4   Diffuse;
    float4   Specular;
    float4   Emissive;
    float    Shiness;
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

cbuffer cbPass : register(b3)
{
    Light Lights[MaxLights];
};

struct VertexIn
{
	float3 Pos    : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
	float2 TexC    : TEXCOORD;
#ifdef SKINNED
    float4 BoneWeights : WEIGHTS;
    int4 BoneIndices  : BONEINDICES;
#endif
};

struct VertexOut
{
	float4 position    : SV_POSITION;
    float3 positionWorld    : POSITION;
    float3 normal : NORMAL;
	float2 texUV    : TEXCOORD;
};

float3 GetCameraPosition()
{
    return ViewInv._41_42_43;
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

#ifdef SKINNED
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; i++)
    {
        if (vin.BoneIndices[i] == -1)
        {
            if (i == 0)
                posL = vin.Pos;
            break;
        }
        posL += vin.BoneWeights[i] * mul(float4(vin.Pos, 1.0f), BoneTransforms[vin.BoneIndices[i]]).xyz;
    }

    vin.Pos = posL;
#endif
	
    float4 posW = mul(float4(vin.Pos, 1.0f), World);
    vout.positionWorld = posW.xyz;

    vout.normal = mul(vin.Normal, (float3x3)World);

    vout.position = mul(posW, ViewProj);
	
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), TexTransform);
	vout.texUV = mul(texC, MatTransform).xy;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 totalResult;
    float3 eyePos = GetCameraPosition();
    float4 albedo = DiffuseMap.Sample(samAnisotropicWrap, pin.texUV);
    float3 eyeDir = normalize(eyePos - pin.positionWorld);
    pin.normal = normalize(pin.normal);

    Material mat = { Ambient, Diffuse, Specular, Emissive, Shiness };
    totalResult = ComputeLight(Lights[0], mat, albedo, pin.normal, eyeDir);

    return totalResult;
}
