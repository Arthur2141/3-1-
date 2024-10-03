struct VS_OUT
{
	float4 position : SV_POSITION;
    float3 wposition : POSITION;
    float3 wnomal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    float3 vTex : TEXCOORD1;
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


static const float Bias = 0.0008;


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
    float d = st.SampleLevel(ss, Tex.xy,2).r;
	// �V���h�E�}�b�v�̐[�x�l�ƌ����̐[�x�̔�r
    Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
    return Scolor;
}

////�V���h�[�}�b�v����V���h�[��ԍ��W�ɕϊ���Z�l��r
//float3 GetShadowPCF(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
//{
//	// �V���h�E�}�b�v����[�x�����o��
//    float2 uv = Tex.xy;
//    float z = Tex.z;
//    float d = st.SampleCmpLevelZero(ss,uv,z,2).r;
//	// �V���h�E�}�b�v�̐[�x�l�ƌ����̐[�x�̔�r
//    Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
//    return Scolor;
//}







////�V���h�[�}�b�v���烉�C�g��ԍ��W�ɕϊ���Z�l��r
//float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
//{
//    float2 d = st.Sample(ss, Tex.xy).rg;
//#if	0	//	�V���v���o�[�W����
//	//	�`�F�r�V�F�t�̕s����
//    float v = max(Bias, d.y - d.x * d.x);
//    //float v = max(0.0f, d.y - d.x * d.x);
//    float e = Tex.z - d.x;
//    float s = saturate(v / (v + e * e));

//    return Scolor.rgb + (1.0f - Scolor.rgb) * s;
    
//#elif	1
//	//	�Z���t�V���h�E�������Ȃ��Ɖ����̂ŁA�[�x������l������
//    float v = max(Bias, d.y - d.x * d.x);

//    //float v = max(0.0f, d.y - d.x * d.x);
//    float e = Tex.z - d.x;
//    float s = saturate(v / (v + e * e));
//    return lerp(Scolor.rgb + (1.0f - Scolor.rgb) * s,1.0f, Tex.z - Bias >= d.x);
//#endif
//}
