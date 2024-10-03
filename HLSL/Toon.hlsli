struct VS_OUT
{
	float4 wposition : POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    float4 wnormal : NORMAL;
};


struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    float4 wposition : POSITION;
    float4 wnormal : NORMAL;
	
};

cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	float4				lightDirection;
    float4				viewPosition;
    
    row_major float4x4 shadowViewProjection;

};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
	float4				materialColor;
};


static const float Bias = 0.0003;
static const float3 ShadowColor = float3(0.7, 0.7, 0.7);


//ワールド座標をライト空間座標に変換
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
	// 正規化デバイス座標系
    float4 wvpPos;
    wvpPos.xyz = wPos;
    wvpPos.w = 1;
    wvpPos = mul(wvpPos, vp);

    wvpPos /= wvpPos.w;
	// テクスチャ座標系
    wvpPos.y = -wvpPos.y;
    wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
    return wvpPos.xyz;
}
//シャドーマップからシャドー空間座標に変換とZ値比較
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
{
	// シャドウマップから深度を取り出す
    float d = st.Sample(ss, Tex.xy).r;
	// シャドウマップの深度値と現実の深度の比較
    Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
    return Scolor;
}

