#include "Default.hlsl"

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
        normalL += vin.BoneWeights[i] * mul(float4(vin.Normal, 1.0f), (float3x3)BoneTransforms[vin.BoneIndices[i]]);
        tangentL +=  vin.BoneWeights[i] * mul(float4(vin.Tangent, 1.0f), (float3x3)BoneTransforms[vin.BoneIndices[i]]);
    }

    vin.Pos = posL;
    vin.Normal = normalL;
    vin.Tangent = tangentL;
#endif

    float4 posW = mul(float4(vin.Pos, 1.0f), World);
    vout.positionWorld = posW.xyz;

    vout.normal = mul(vin.Normal, (float3x3)World);

    vout.position = mul(posW, ViewProj);
	
	// float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), TexTransform);
	vout.texUV = mul(float4(vin.TexC, 0.0f, 1.0f), MatTransform).xy;
	
    return vout;
}