
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 texCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 texCoord : TEXCOORD0;
};

cbuffer constBuffer : register(b0)
{
    matrix worldViewProjMatrix;
    matrix modelMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 worldPos = mul(float4(input.position, 1.0f), modelMatrix);

    output.position = mul(worldPos, worldViewProjMatrix);
    output.worldPosition = worldPos.xyz;
    output.normal = normalize(mul(float4(input.normal, 0.0f), modelMatrix).xyz);
    output.tangent = normalize(mul(float4(input.tangent, 1.0f), modelMatrix).xyz);
    output.bitangent = normalize(mul(float4(input.bitangent, 1.0f), modelMatrix).xyz);
    output.texCoord = input.texCoord;
    
    return output;
}