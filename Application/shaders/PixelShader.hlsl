
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

float4 CalculateDirectionalLight(float3 normal)
{
    float3 norm = normalize(normal);
    float3 dir = normalize(-lightDirection.xyz);
    float3 diff = max(dot(norm, dir), 0.0f);
    
    float3 ambient = lightColor.rgb * lightAmbient.rgb;
    float3 diffuse = lightColor.rgb * lightDiffuse.rgb * diff;
    
    return float4(ambient + diffuse, 1.0f);
}

float4 main(PS_INPUT input) : SV_Target
{
    float4 light = CalculateDirectionalLight(input.normal);
    float4 texColor = shaderTexture.Sample(sampleType, input.texCoord);
    return texColor * light;
}