struct VS_OUT {
    float3  Position : POSITION;
    float4  Color : COLOR;
    uint    TexIndex : TEXINDEX;
    float2  Size : SIZE;
};

struct GS_OUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    uint texIndex : TEXINDEX;
};