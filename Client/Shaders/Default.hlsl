//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
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

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 World;
	float4x4 TexTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4 AmbientLight;

    Light Lights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
    float4   Ambient;
	float4   DiffuseAlbedo;
    float3   FresnelR0;
    float    Roughness;
	float4x4 MatTransform;
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
}

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
    float3 Tangent : TANGENT;
	float2 TexC    : TEXCOORD;
#ifdef SKINNED
    float4 BoneWeights : WEIGHTS;
    int4 BoneIndices  : BONEINDICES;
#endif
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
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
                posL = vin.PosL;
            break;
        }
        posL += vin.BoneWeights[i] * mul(float4(vin.PosL, 1.0f), BoneTransforms[vin.BoneIndices[i]]).xyz;
    }

    vin.PosL = posL;
#endif
	
    float4 posW = mul(float4(vin.PosL, 1.0f), World);
    vout.PosW = posW.xyz;

    vout.NormalW = mul(vin.NormalL, (float3x3)World);

    vout.PosH = mul(posW, ViewProj);
	
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), TexTransform);
	vout.TexC = mul(texC, MatTransform).xy;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float3 eyePos = GetCameraPosition();
    float4 diffuseAlbedo = DiffuseMap.Sample(samAnisotropicWrap, pin.TexC) * DiffuseAlbedo;
	
    pin.NormalW = normalize(pin.NormalW);

    float3 toEyeW = normalize(eyePos - pin.PosW);

    float4 ambient = AmbientLight*diffuseAlbedo;

    const float shininess = 1.0f - Roughness;
    float3 shadowFactor = 1.0f;

    float4 litColor = ambient;
    litColor.a = diffuseAlbedo.a;

    return litColor;
}
