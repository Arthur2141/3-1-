#include "PhongDistruct.hlsli"


[maxvertexcount(3)]
void main(
	triangle VS_OUT input[3],
	inout TriangleStream<GS_OUT> output
)
{
    // –Ê–@ü‚ÌŒvZ
    float3 v1 = input[1].wposition - input[0].wposition;
    float3 v2 = input[2].wposition - input[0].wposition;
    float3 n = normalize(cross(v1, v2));

	for (uint i = 0; i < 3; i++)
	{
        GS_OUT element;
        float3 wposition = input[i].wposition + n * 0.2f;
        element.position = mul(float4(wposition, 1.0f), viewProjection);

		element.wposition = wposition;
        element.texcoord = input[i].texcoord;
        element.color = input[i].color;
        element.wnomal = input[i].wnomal;
		output.Append(element);
	}
    output.RestartStrip();

}