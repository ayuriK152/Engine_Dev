#include "Skybox.hlsl"

float4 PS(VertexSkyboxOut pin) : SV_Target
{
	return CubeMap.Sample(samLinearWrap, pin.LocalPosition);
}