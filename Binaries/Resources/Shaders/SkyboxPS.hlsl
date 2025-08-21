#include "Skybox.hlsl"

float4 PS(VertexOut pin) : SV_Target
{
	return CubeMap.Sample(samLinearWrap, pin.LocalPosition);
}