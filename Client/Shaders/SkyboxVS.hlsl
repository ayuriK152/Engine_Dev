#include "Skybox.hlsl"
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Use local vertex position as cubemap lookup vector.
	vout.LocalPosition = vin.Position;
	
	// Transform to world space.
	float4 posW = mul(float4(vin.Position, 1.0f), World);

	// Always center sky about camera.
	posW.xyz += GetCameraPosition();

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	vout.Position = mul(posW, ViewProj).xyww;
	
	return vout;
}