#include "Structs.hlsl"

VertexTexOut VS(VertexTexIn vin)
{
    VertexTexOut vout;
    // 화면 좌표 그대로 사용 (정규화된 -1~1)
    vout.PosH = float4(vin.PosL, 1.0f);
    vout.TexC = vin.TexC;
    return vout;
}