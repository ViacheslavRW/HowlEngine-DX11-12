#define MAX_POINT_LIGHTS 2
#define PI 3.1415926

Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D ormMap : register(t2);

SamplerState sampleType : register(s0);

cbuffer DirectionalLightBuffer : register(b1)
{
    float4 lightDirection;
    float4 lightColor;
    float4 lightAmbient;
    float4 lightDiffuse;
};

struct PointLight
{
    float3 lightPosition;
    uint active;
    float4 lightColor;
    float4 lightAmbient;
    float4 lightDiffuse;
    float3 lightAttenuation;
    float lightRange;
};

cbuffer PointLightBuffer : register(b2)
{
    PointLight pointLights[MAX_POINT_LIGHTS];
}

cbuffer CameraBuffer : register(b3)
{
    float3 cameraPosition;
    float pad;
}

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 texCoord : TEXCOORD;
};

float3 GetNormalFromMap(PS_INPUT input)
{
    float3 tangentNormal = normalMap.Sample(sampleType, input.texCoord).rgb * 2.0f - 1.0f;
    //tangentNormal.y *= -1.0f; // Flip Y if needed
    float3 T = normalize(input.tangent);
    float3 N = normalize(input.normal);
    float3 B = normalize(input.bitangent);
    
    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(tangentNormal, TBN));
}

float3 FresnelSchlick(float cosTheta, float3 f0)
{
    return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
    
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    return a2 / (PI * denom * denom + 0.0001f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;
    return NdotV / (NdotV * (1.0f - k) + k + 0.0001f);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 CalculateLight(float3 L, float3 H, float3 V, float3 N, float3 lightColor, float3 albedo, float metallic, float roughness)
{
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);

    float3 numerator = NDF * G * F;
    float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
    float3 specular = numerator / denominator;

    float3 kS = F;
    float3 kD = (1.0f - kS) * (1.0f - metallic);

    float NdotL = max(dot(N, L), 0.0f);

    return (kD * albedo / PI + specular) * lightColor * NdotL;
}

float4 main(PS_INPUT input) : SV_Target
{
    float4 albedoSample = albedoMap.Sample(sampleType, input.texCoord);
    float3 albedo = pow(albedoSample.rgb, 2.2f);
    float alpha = albedoSample.a;
    
    float3 orm = ormMap.Sample(sampleType, input.texCoord).rgb;
    float occlusion = orm.r * 0.8f;
    float roughness = orm.g;
    float metallic = orm.b;

    float3 N = GetNormalFromMap(input);
    float3 V = normalize(cameraPosition - input.worldPosition);
    
    // Ensure two-sided lighting
    float NdotV = dot(N, V);
    if (NdotV < 0.0f) N = -N;

    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    float3 ambient = lightAmbient.rgb * albedo * occlusion;
    
    float3 Ld = normalize(lightDirection.xyz);
    float3 Hd = normalize(V + Ld);
    Lo += CalculateLight(Ld, Hd, V, N, lightColor.rgb, albedo, metallic, roughness);

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (pointLights[i].active == 0)
            continue;
        
        float3 toLight = pointLights[i].lightPosition - input.worldPosition;
        float dist = length(toLight);
        if (dist > pointLights[i].lightRange)
            continue;
        
        float3 L = normalize(toLight);
        float3 H = normalize(V + L);
        
        float atten = 1.0f / (pointLights[i].lightAttenuation.x +
                             pointLights[i].lightAttenuation.y * dist +
                             pointLights[i].lightAttenuation.z * dist * dist + 0.0001f);
        
        Lo += CalculateLight(L, H, V, N, pointLights[i].lightColor.rgb, albedo, metallic, roughness) * atten;
    }

    float3 color = ambient + Lo;
    color = pow(color, 1.0f / 2.2f);

    return float4(color, alpha);
}