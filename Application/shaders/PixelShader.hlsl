
Texture2D shaderTexture : register(t0);
SamplerState sampleType : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target
{
    return shaderTexture.Sample(sampleType, input.texCoord);
}