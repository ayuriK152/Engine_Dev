#include "Structs.hlsl"

float4 PS(VertexColorOut pin) : SV_Target
{
    return pin.Color;
}