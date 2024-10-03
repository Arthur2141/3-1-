#include "WaterSurface.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

Texture2D shadowMap : register(t1);
SamplerState shadowMapSamplerState : register(s1);

Texture2D EnvMap : register(t2);
Texture2D ReflectMap : register(t3);//�ǉ�

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 P = pin.wposition.xyz;
    float3 E = normalize(viewPosition.xyz - P);
    float3 L = normalize(lightDirection.xyz);
    float3 N = normalize(pin.wnomal.xyz);
    
    //	���˃x�N�g���𐳋��~���}�@�ɑ�����UV���W�֕ϊ�����
    float3 RE = normalize(reflect(-E, N));
    float2 texcoord;
    {
        texcoord = float2(1.0f - atan2(RE.z, RE.x) / 2.0f, -atan2(RE.y, length(RE.xz)));
        texcoord = texcoord / PI + 0.5f;
    }

    // �g�̃p�����[�^
    const float2 speeds[2] =
    {
        { 0.002f, 0.001f },
        { -0.001f, 0.002f }
    };
    const float wave_size[2] = { 15.0f, 15.0f };
    const float wave_influence = 0.01f;
    
    // �g�̖@���Z�o
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
    
        temp1 = normalize(temp1 + temp0);
    
        normal.x = temp1.r;
        normal.y = temp1.b;
        normal.z = temp1.g;
    }




    float4 color=(float4)0;
    {
        float4 envColor = EnvMap.Sample(diffuseMapSamplerState, 
            texcoord + normal.xz * wave_influence);

        // �t���l��
        float alpha = saturate(1.0 - dot(E, normal));
        color.a = alpha;
        color.rgb = envColor.rgb * 0.8f * alpha;
    }
    // �e
    if (dot(-L, N) > 0)
    {
        float3 wTex = GetShadowTex(shadowViewProjection, P);
        float3 shadow_color = GetShadow(shadowMap, shadowMapSamplerState, wTex, Scolor, Bias);

        color.rgb *= shadow_color;
    }
    {
        //�@�I�u�W�F�N�g�f�荞�݂̐F�擾
        float3 wTex = GetShadowTex(reflectViewProjection, P);
        // �g�̉e���i���߁j
        float4 reflect_color = ReflectMap.Sample(diffuseMapSamplerState,
            wTex.xy + normal.xz * wave_influence * 1.0f);
        // ���t���N�V�����}�b�v�̍��F�ȊO�̉f�荞��
        if (any(reflect_color.rgb))
        {
            color.rgb = reflect_color.rgb;
        }

    }
     //	���ʔ���
    {  
        float3 R = reflect(L, normal);
        float S = pow(max(0, dot(R, E)), 16);
        color.rgba += S;
    }

    return color;
}

