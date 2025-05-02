struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct MaterialData
{
    float4 diffuseAlbedo;
};

ConstantBuffer<MaterialData> gMaterialData : register(b1);

float4 main(PS_INPUT input) : SV_Target
{
    return gMaterialData.diffuseAlbedo;
    //return input.color;
}