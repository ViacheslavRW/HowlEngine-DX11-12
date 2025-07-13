#define MAX_POINT_LIGHTS 2

Texture2D shaderTexture : register(t0);
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
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

float4 CalculateDirectionalLight(float3 normal)
{
    float3 norm = normalize(normal);
    float3 dir = normalize(-lightDirection.xyz);
    float3 diff = max(dot(norm, dir), 0.0f);
    
    float3 ambient = lightColor.rgb * lightAmbient.rgb;
    float3 diffuse = lightColor.rgb * lightDiffuse.rgb * diff;
    
    return float4(ambient + diffuse, 1.0f);
}

float4 CalculatePointLight(float3 normal)
{
    float3 finalLight = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        PointLight light = pointLights[i];

        if (light.active == 0) continue;
        
        float3 toLight = light.lightPosition.xyz;
        float dist = length(toLight);
        float3 dir = normalize(toLight);

        float attenuation = 1.0 / (light.lightAttenuation.x + light.lightAttenuation.y * dist + light.lightAttenuation.z * dist * dist);
        attenuation *= saturate(1.0 - dist / light.lightRange);

        float NdotL = max(dot(normalize(normal), dir), 0.0f);

        float3 ambient = light.lightAmbient.rgb * light.lightColor.rgb;
        float3 diffuse = light.lightDiffuse.rgb * light.lightColor.rgb * NdotL;

        finalLight += (ambient + diffuse) * attenuation;
    }

    return float4(finalLight, 1.0f);
}

float4 main(PS_INPUT input) : SV_Target
{
    float4 dirLight = CalculateDirectionalLight(input.normal);
    float4 pointLight = CalculatePointLight(input.normal);
    float4 texColor = shaderTexture.Sample(sampleType, input.texCoord);
    return texColor * (dirLight + pointLight);
}