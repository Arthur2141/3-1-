struct VS_OUT
{
	float4 position : SV_POSITION;
    float3 wposition : POSITION;
    float3 wnomal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
};

cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	float4				lightDirection;
    float4				viewPosition;
	row_major float4x4	shadowViewProjection;
    float4				param; // x:timer
	row_major float4x4	reflectViewProjection;//�ǉ�
    float4				param2; // xyz:ripples_position,w:ripples_size

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

static const float3 Scolor = float3(0.9, 0.9, 0.9);
static const float Bias = 0.0008;
static const float PI = 3.141592654f;


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

#if 1

//�V���h�[�}�b�v����V���h�[��ԍ��W�ɕϊ���Z�l��r
float3 GetShadow(Texture2D st, SamplerState ss, 
	float3 Tex, float3 Scolor, float Bias)
{
	// �V���h�E�}�b�v����[�x�����o��
	float d = st.Sample(ss, Tex.xy).r;
	// �V���h�E�}�b�v�̐[�x�l�ƌ����̐[�x�̔�r
	Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
	return Scolor;
}




#else
float3 GetShadow(Texture2D st, SamplerState ss, 
	float3 Tex, float3 Scolor, float Bias) {
	float2 d = st.Sample(ss, Tex.xy).rg;
	// �`�F�r�V�F�t�̕s����
	float v = min(Bias, d.y - d.x * d.x);
	float e = Tex.z - d.x;
	float s = saturate(v / (v + e));
	return Scolor + (1.0f - Scolor) * s;
}
#endif