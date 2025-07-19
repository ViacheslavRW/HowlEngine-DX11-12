
Texture2D shaderTexture : register(t0);
SamplerState sampleType : register(s0);
cbuffer DirectionalLightBuffer : register(b1)
{
    float4 lightDirection;
    float4 lightColor;
    float4 lightAmbient;
    float4 lightDiffuse;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target
{
    float3 norm = normalize(input.normal);
    float3 lightDir = normalize(-lightDirection.xyz);
    float3 viewDir = normalize(input.position.xyz);

    float NdotL = max(dot(norm, lightDir), 0.0f);
    
    float3 ambient = lightAmbient.rgb * lightColor.rgb;
    float3 diffuse = lightDiffuse.rgb * lightColor.rgb * NdotL;
    float3 lighting = ambient + diffuse;

    float4 texColor = shaderTexture.Sample(sampleType, input.texCoord);
    float4 tintColor = float4(1.0f, 1.0f, 1.0f, 0.2f);

    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
    fresnel = saturate(fresnel * 1.5);

    float3 finalColor = texColor.rgb * lighting + fresnel;

    return float4(finalColor, texColor.a * tintColor.a);
}