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


//���[���h���W�����C�g��ԍ��W�ɕϊ�
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
	// ���K���f�o�C�X���W�n
    float4 wvpPos;
    wvpPos.xyz = wPos;
    wvpPos.w = 1;
    wvpPos = mul(wvpPos, vp);

    wvpPos /= wvpPos.w;
	// �e�N�X�`�����W�n
    wvpPos.y = -wvpPos.y;
    wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
    return wvpPos.xyz;
}
//�V���h�[�}�b�v����V���h�[��ԍ��W�ɕϊ���Z�l��r
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
{
	// �V���h�E�}�b�v����[�x�����o��
    float d = st.Sample(ss, Tex.xy).r;
	// �V���h�E�}�b�v�̐[�x�l�ƌ����̐[�x�̔�r
    Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
    return Scolor;
}

