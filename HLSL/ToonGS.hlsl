#include "Toon.hlsli"


[maxvertexcount(6)]
void main(
	triangle VS_OUT input[3],
	inout TriangleStream< GS_OUT > output
)
{
	GS_OUT element=(GS_OUT)0;
    int i;
	for (i = 0; i < 3; i++)
	{
		element.position = mul(input[i].wposition, viewProjection);
        element.wposition = input[i].wposition;
        element.wnormal = input[i].wnormal;
		element.texcoord = input[i].texcoord;
		element.color = input[i].color;	
		output.Append(element);
	}
    output.RestartStrip();
	
	//	���ʂŊg�債�ďo�͂��쐬
    for (i = 2; i >= 0; --i)
    {
        float dist = length(viewPosition.xyz - input[i].wposition.xyz);
        float4 P = input[i].wposition + input[i].wnormal * (0.01f + saturate(dist / 1000.0f));
        element.position = mul(P, viewProjection);
        element.wposition = P;
        element.wnormal = input[i].wnormal;
        element.texcoord = input[i].texcoord;
        element.color = float4(0,0,0, 1);
		//	�����Œ��_�𐶐����Ă���
        output.Append(element);
    }

	//	�ǉ��������_�Ńv���~�e�B�u���\�����A�V�����v���~�e�B�u�𐶐�����
    output.RestartStrip();

}