
#include "Toon.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

Texture2D rampTexture : register(t1);

Texture2D shadowMap : register(t2);
SamplerState shadowMapSamplerState : register(s2);


float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord);
    float bright = rampTexture.Sample(diffuseMapSamplerState, pin.texcoord);
    //color *= pin.color;
    float3 P = pin.wposition.xyz;
    float3 N = normalize(pin.wnormal.xyz);
    float3 L = normalize(lightDirection.xyz);
   
    float3 E = normalize(viewPosition.xyz - P);
    
    //  ä¬ã´åı
    float3 A;
    {
        A = float3(0.4, 0.4, 0.4);

    }
    
    //	ägéUîΩéÀ
    float3 D;
	{
        float U = dot(-L, N) * 0.5f + 0.5f;
        D = rampTexture.Sample(diffuseMapSamplerState, float2(U, 1)).rgb;

    }

	//	ãæñ îΩéÀ
    float3 S;
	{
        float U = dot(normalize(reflect(L, N)), E);
        S = rampTexture.Sample(diffuseMapSamplerState, float2(U, 1)).rgb;
        S = pow(saturate(S), 20) * 0.9f;
    }

    color.rgb *= A + D + S;
    
    //  ó÷äsê¸
    color.rgb *= pin.color;
         
    float3 tex = GetShadowTex(shadowViewProjection, P);
    float3 shadowColor = GetShadow(shadowMap, shadowMapSamplerState, tex, ShadowColor, Bias);
    //float3 shadowColor = GetShadow(shadowMap, shadowMapSamplerState, pin.vTex, ShadowColor, Bias);
    
    color.rgb *= shadowColor;

	return color;
}

