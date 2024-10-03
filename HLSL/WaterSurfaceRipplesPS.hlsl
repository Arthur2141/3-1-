#include "WaterSurfaceRipples.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

Texture2D shadowMap : register(t1);
SamplerState shadowMapSamplerState : register(s1);
SamplerState ripplesshadowMapSamplerState : register(s2);

Texture2D EnvMap : register(t2);
Texture2D ReflectMap : register(t3);//追加

Texture2D RipplesMap : register(t4);//追加

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 P = pin.wposition.xyz;
    float3 E = normalize(viewPosition.xyz - P);
    float3 L = normalize(lightDirection.xyz);
    float3 N = normalize(pin.wnomal.xyz);
    
    //	反射ベクトルを正距円筒図法に則ったUV座標へ変換する
    float3 RE = normalize(reflect(-E, N));
    float2 texcoord;
    {
        texcoord = float2(1.0f - atan2(RE.z, RE.x) / 2.0f, -atan2(RE.y, length(RE.xz)));
        texcoord = texcoord / PI + 0.5f;
    }

    // 波のパラメータ
    const float2 speeds[2] =
    {
        { 0.002f, 0.001f },
        { -0.001f, 0.002f }
    };
    const float wave_size[2] = { 10.0f, 10.0f };
    const float wave_influence = 0.01f;
    
    // 波の法線算出
    float3 normal;
    {
        float2 tex = 0;
        tex.x = pin.texcoord.x + param.x * speeds[0].x;
        tex.y = pin.texcoord.y + param.x * speeds[0].y;
        float3 temp0 = diffuseMap.Sample(diffuseMapSamplerState, tex * wave_size[0]).rgb;
        temp0 = (temp0 - 0.5f) * 2.0f;
    
        tex.x = pin.texcoord.x + param.x * speeds[1].x;
        tex.y = pin.texcoord.y + param.x * speeds[1].y;
        float3 temp1 = diffuseMap.Sample(diffuseMapSamplerState, tex * wave_size[1]).rgb;
        temp1 = (temp1 - 0.5f) * 2.0f;
        
        float4 temp2 = float4(0, 0, 0, 1);
        {
            float size = param2.w * 0.5f * wave_size[0];
            float2 p = param2.xz;
        
            float com;
            com = step(p.x - size, P.x);
            com = step(P.x, p.x + size) * com;
            com = step(p.y - size, P.z) * com;
            com = step(P.z, p.y + size) * com;
        
            float4 t = RipplesMap.Sample(ripplesshadowMapSamplerState, (P.xz - p) * 0.5 / size + 0.5);
            t.rgb = (t.rgb - 0.5f) * 2.0f;
            temp2.r = t.r;
            temp2.g = t.g;
            temp2.b = -t.b;
            
        }

  
        temp1 = normalize(temp0 + temp1 + temp2.rgb);
    
        normal.x = temp1.r;
        normal.y = temp1.b;
        normal.z = temp1.g;
    }


    float4 color=(float4)0;
    {
        float4 envColor = EnvMap.Sample(diffuseMapSamplerState, 
            texcoord + normal.xz * wave_influence);

        // フレネル
        float alpha = saturate(1.0 - dot(E, normal));
        color.a = alpha;
        color.rgb = envColor.rgb * 0.8f * alpha;
    }
    // 影
    if (dot(-L, N) > 0)
    {
        float3 wTex = GetShadowTex(shadowViewProjection, P);
        float3 shadow_color = GetShadow(shadowMap, shadowMapSamplerState, wTex, Scolor, Bias);

        color.rgb *= shadow_color;
    }
    {
        //　オブジェクト映り込みの色取得
        float3 wTex = GetShadowTex(reflectViewProjection, P);
        // 波の影響（強め）
        float4 reflect_color = ReflectMap.Sample(diffuseMapSamplerState,
            wTex.xy + normal.xz * wave_influence * 1.0f);
        // リフレクションマップの黒色以外の映り込み
        if (any(reflect_color.rgb))
        {
            color.rgb = reflect_color.rgb;
        }
    }
    
     //	鏡面反射
    {  
        float3 R = reflect(L, normal);
        float S = pow(max(0, dot(R, E)), 16);
        color.rgba += S;
    }
    return color;
}

