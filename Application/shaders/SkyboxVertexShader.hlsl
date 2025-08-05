
struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD0;
};

cbuffer constBuffer : register(b0)
{
    matrix viewProjMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.position = mul(float4(input.position, 1.0f), viewProjMatrix);
    output.texCoord = input.position;
    
    return output;
}