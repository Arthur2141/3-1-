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
	
	//	裏面で拡大して出力を作成
    for (i = 2; i >= 0; --i)
    {
        float dist = length(viewPosition.xyz - input[i].wposition.xyz);
        float4 P = input[i].wposition + input[i].wnormal * (0.01f + saturate(dist / 1000.0f));
        element.position = mul(P, viewProjection);
        element.wposition = P;
        element.wnormal = input[i].wnormal;
        element.texcoord = input[i].texcoord;
        element.color = float4(0,0,0, 1);
		//	ここで頂点を生成している
        output.Append(element);
    }

	//	追加した頂点でプリミティブを構成し、新しいプリミティブを生成する
    output.RestartStrip();

}