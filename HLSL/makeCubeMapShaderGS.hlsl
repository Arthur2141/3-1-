#include "makeCubeMapShader.hlsli"


//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------

[maxvertexcount(18)]
void main(
	triangle GSInput input[3],
	inout TriangleStream< PSInput > output
)
{
	for (int f = 0; f < 6; f++) 
	{
		PSInput element = (PSInput)0;
		element.RTIndex = f;
		for (int i = 0; i < 3; i++)
		{
			////�@�r���[�E���e�ϊ�
			//float4 P = mul(ViewMat[f], input[i].Position);
			//element.Position = mul(Projection, P);
			
			//�@�r���[�E���e�ϊ�(row_major)
            float4 P = mul(input[i].Position, ViewMat[f]);
            element.Position = mul(P, Projection);
			
			
			//�@���[���h�@��
			element.wNormal = normalize(input[i].Normal).xyz;

			element.Color = input[i].Color;

			//�e�N�X�`���[���W
			element.Tex = input[i].Tex;
			//�ǉ�
			output.Append(element);
		}
		output.RestartStrip();
	}
}