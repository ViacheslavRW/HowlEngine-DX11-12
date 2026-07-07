Texture2D hdrTexture : register(t0);
SamplerState sampleType : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};


float4 main(PS_INPUT input) : SV_Target
{
    float3 dir = normalize(input.texCoord);
    
    float2 uv;
    uv.x = atan2(dir.z, dir.x) * 0.5f / 3.1415926f + 0.5f;
    uv.y = acos(dir.y) / 3.1415926f;
    
    float4 color = hdrTexture.Sample(sampleType, uv);
    
    return float4(color.rgb, 1.0f);
}