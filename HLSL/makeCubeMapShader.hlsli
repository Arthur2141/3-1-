struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
};


cbuffer CBPerCubeMap : register(b3)
{
    row_major float4x4 ViewMat[6]; //カメラ行列
    row_major float4x4 Projection; //投影変換
};

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//頂点カラー無し
};

//ジオメトリシェーダー出力構造体
struct GSInput
{
	float4 Position : SV_POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//頂点カラー無し
};


//バーテックスシェーダー出力構造体
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//ワールド法線
	float3 wPosition : TEXCOORD2;	//ワールド座標
	float2 Tex : TEXCOORD3;
	uint   RTIndex:	SV_RenderTargetArrayIndex;
};

