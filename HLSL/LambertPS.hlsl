
#include "Lambert.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	//return float4(1,1,0,1);
	return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
}

