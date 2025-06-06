
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PassData
{
    float4x4 viewProjection;
};

ConstantBuffer<PassData> gPassData : register(b0);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(gPassData.viewProjection, float4(input.position, 1.0f));
    output.color = input.color;
    
    return output;
}