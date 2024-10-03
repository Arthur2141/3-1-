
#include "ShadowMap.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color;
    float depth = pin.depth.z / pin.depth.w;
    color.r = depth;
    color.g = depth * depth;
    color.b = 1.0f;
    color.a = 1.0f;

    return color;
	
}

