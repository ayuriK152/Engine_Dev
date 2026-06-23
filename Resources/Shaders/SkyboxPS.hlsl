#include "Skybox.hlsl"

float4 PS(VertexSkyboxOut pin) : SV_Target {
	return CubeMap.Sample(samLinearWrap, pin.LocalPosition) * (1.0 - CameraColorBlend.a) + CameraColorBlend * CameraColorBlend.a;
}