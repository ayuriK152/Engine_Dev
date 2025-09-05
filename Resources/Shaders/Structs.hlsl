#define MAX_KEYFRAME_COUNT 512
#define MAX_BONE_COUNT 128

/****************/
/* Skinned Mesh */
/****************/

struct BoneTransform
{
    float4x4 transform;
};

struct KeyFrame
{
    float tick;
    float3 pos;
    float4 rot;
    float3 scl;
};

struct AnimationData
{
    KeyFrame keyFrames[MAX_KEYFRAME_COUNT];
    uint keyFrameCount;
};

struct Animation
{
    AnimationData animationDatas[MAX_BONE_COUNT];
    uint boneCount;
};

/*********/
/* Light */
/*********/

struct Light
{
    float4x4 View;
    float4x4 Proj;
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float3  Direction;
    int     LightType;
    float2  FalloffInfo;
    float   SpotPower;
    int     padding1;
};

struct Material
{
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float4  Emissive;
    float   Shiness;
};

/**********/
/* Vertex */
/**********/

struct VertexIn
{
	float3 Pos          : POSITION;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
	float2 TexC         : TEXCOORD;
#ifdef SKINNED
    float4 BoneWeights  : WEIGHTS;
    int4 BoneIndices    : BONEINDICES;
#endif
};

struct VertexOut
{
	float4 position         : SV_POSITION;
    float3 positionWorld    : POSITION;
    float3 normal           : NORMAL;
	float2 texUV            : TEXCOORD2;
    float4 shadowPos        : TEXCOORD1;
};

struct VertexColorIn
{
    float3 Pos          : POSITION;
    float4 Color        : COLOR;
};

struct VertexColorOut
{
    float4 Position     : SV_POSITION;
    float4 Color        : COLOR;
};

struct VertexTexIn
{
    float3 PosL : POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexTexOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexPosIn
{
    float3 Pos : POSITION;
};

struct VertexPosOut
{
    float4 Pos : SV_POSITION;
};