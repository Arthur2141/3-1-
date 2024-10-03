
#include "PhongDrawShadow.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

Texture2D shadowMap : register(t1);
SamplerState shadowMapSamplerState : register(s1);


float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord);
    color *= pin.color;
    float3 E = normalize(viewPosition.xyz-pin.wposition);
    float3 L = normalize(lightDirection.xyz);
    float3 N = normalize(pin.wnomal);
    float3 R = reflect(L, N);
    float S = pow(max(0, dot(R, E)), 20);
    color.rgb += S;
    
    
    //float3 tex = GetShadowTex(shadowViewProjection, pin.wposition);
    float3 shadowColor = GetShadow(shadowMap, shadowMapSamplerState, pin.vTex, ShadowColor, Bias);
    if(dot(-L,N)>0) color.rgb *= shadowColor;
     
        return color;
    }

