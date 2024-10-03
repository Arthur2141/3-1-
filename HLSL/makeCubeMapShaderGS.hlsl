#include "makeCubeMapShader.hlsli"


//--------------------------------------------
//	ピクセルシェーダー
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
			////　ビュー・投影変換
			//float4 P = mul(ViewMat[f], input[i].Position);
			//element.Position = mul(Projection, P);
			
			//　ビュー・投影変換(row_major)
            float4 P = mul(input[i].Position, ViewMat[f]);
            element.Position = mul(P, Projection);
			
			
			//　ワールド法線
			element.wNormal = normalize(input[i].Normal).xyz;

			element.Color = input[i].Color;

			//テクスチャー座標
			element.Tex = input[i].Tex;
			//追加
			output.Append(element);
		}
		output.RestartStrip();
	}
}