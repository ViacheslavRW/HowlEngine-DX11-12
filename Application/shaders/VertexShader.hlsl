
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

cbuffer constBuffer : register(b0)
{
    matrix worldViewProjmatrix;
};

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output;
    
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, worldViewProjmatrix);
    output.normal = input.normal;
    output.texCoord = input.texCoord;
    
    return output;
}