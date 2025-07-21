#define MAX_POINT_LIGHTS 2
Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallicMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D aoMap : register(t4);
Texture2D ormMap : register(t5);

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
    
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.bitangent);
    float3 N = normalize(input.normal);
    
    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(tangentNormal, TBN));
}

float4 main(PS_INPUT input) : SV_Target
{
    float3 albedo = albedoMap.Sample(sampleType, input.texCoord).rgb;
    float3 orm = ormMap.Sample(sampleType, input.texCoord).rgb;
    //float metallic = metallicMap.Sample(sampleType, input.texCoord).r;
    //float roughness = roughnessMap.Sample(sampleType, input.texCoord).r;
    //float ao = aoMap.Sample(sampleType, input.texCoord).r;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;

    // Compute normal from normal map and interpolate with vertex normal
    float3 norm = GetNormalFromMap(input);

    // Directional light
    float3 lightDir = normalize(-lightDirection.xyz);
    float NdotL = saturate(dot(norm, lightDir));

    float3 ambient = lightAmbient.rgb * ao;
    float3 diffuse = lightColor.rgb * NdotL * (1.0f - metallic);

    // Point lights
    float3 pointLightSum = float3(0, 0, 0);
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        PointLight light = pointLights[i];
        if (light.active == 0)
            continue;

        float3 toLight = light.lightPosition - input.worldPosition;
        float dist = length(toLight);
        if (dist > light.lightRange)
            continue;

        float3 L = normalize(toLight);
        float atten = 1.0f / (light.lightAttenuation.x + light.lightAttenuation.y * dist + light.lightAttenuation.z * dist * dist);
        atten *= saturate(1.0f - dist / light.lightRange);
        
        float NdotLPoint = saturate(dot(norm, L));
        
        float3 ambientPoint = light.lightAmbient.rgb * light.lightColor.rgb;
        float3 diffusePoint = light.lightDiffuse.rgb * light.lightColor.rgb * NdotLPoint;

        pointLightSum += (ambientPoint + diffusePoint) * atten;
    }

    float3 finalColor = albedo * (ambient + diffuse + pointLightSum);

    return float4(finalColor, 1.0f);
    //return float4(norm * 0.5f + 0.5f, 1.0f); // visualize normals
    //return float4(ao, ao, ao, 1); // visualize ao
    //return float4(roughness, roughness, roughness, 1); // visualize roughness
    //return float4(metallic, metallic, metallic, 1); // visualize metallic
}