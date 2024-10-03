
#include "Phong.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord);
    color *= pin.color;
    float3 P = pin.wposition.xyz;
    float3 E = normalize(viewPosition.xyz - P);
    float3 L = normalize(lightDirection.xyz);
    float3 N = normalize(pin.wnomal.xyz);
    float3 R = reflect(L, N);
    float S = pow(max(0, dot(R, E)), 20);
    color.rgb += S;
    return color;
}

