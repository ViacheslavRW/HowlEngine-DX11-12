
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
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
    output.color = input.color;
    return output;
}