
struct VSInput
{
    float3 position : POSITION0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput main(VSInput input) 
{
    VSOutput output = (VSOutput) 0;
    output.position = float4(input.position, 1.0f);
    return output;
}