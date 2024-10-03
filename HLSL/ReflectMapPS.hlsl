
#include "ReflectMap.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord);
    float4 P = pin.wposition;
    float4 E = P - viewPosition;
    
    color *= pin.color;
            
        
    { // …–Ê‰º‚ÌƒsƒNƒZƒ‹íœ
        clip(param.x < P.y ? 1 : -1);          
    }
        
    return color;
}

        //if (param.x - 0.1f > P.y)
        //{
        //    return float4(0, 0, 0, 1);           
        //}
