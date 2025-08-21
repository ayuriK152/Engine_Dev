#include "Structs.hlsl"
#include "LightingUtil.hlsl"

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

#ifdef SKINNED
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);
    
    float boneWeights[4];
    boneWeights[0] = vin.BoneIndices[0] == -1 ? 0 : vin.BoneWeights[0];
    boneWeights[1] = vin.BoneIndices[1] == -1 ? 0 : vin.BoneWeights[1];
    boneWeights[2] = vin.BoneIndices[2] == -1 ? 0 : vin.BoneWeights[2];
    boneWeights[3] = vin.BoneIndices[3] == -1 ? 0 : vin.BoneWeights[3];

    float weightNormalizeValue = 1.0f / (boneWeights[0] + boneWeights[1] + boneWeights[2] + boneWeights[3]);

    for (int i = 0; i < 4; i++)
    {
        posL += boneWeights[i] * weightNormalizeValue * mul(float4(vin.Pos, 1.0f), BoneTransforms[vin.BoneIndices[i]]).xyz;
        normalL += boneWeights[i] * weightNormalizeValue * mul(vin.Normal, (float3x3)BoneTransforms[vin.BoneIndices[i]]);
        tangentL +=  boneWeights[i] * weightNormalizeValue * mul(vin.Tangent.xyz, (float3x3)BoneTransforms[vin.BoneIndices[i]]);
    }

    vin.Pos = posL;
    vin.Normal = normalL;
    vin.Tangent.xyz = tangentL;
#endif

    float4 posW;

#ifdef SKINNED
    posW = float4(vin.Pos, 1.0f);
    vout.normal =  vin.Normal;
#else
    posW = mul(float4(vin.Pos, 1.0f), World);
    vout.normal =  normalize(mul(vin.Normal, (float3x3)World));
#endif
    vout.positionWorld = posW.xyz;
    //vout.normal =  normalize(mul(vin.Normal, (float3x3)World));
    vout.position = mul(posW, ViewProj);
	vout.texUV = mul(float4(vin.TexC, 0.0f, 1.0f), MatTransform).xy;

    vout.shadowPos = mul(posW, mul(Lights[0].View, Lights[0].Proj));
	
    return vout;
}