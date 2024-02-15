[[vk::binding(0, 0)]]
cbuffer MVP
{
    matrix<float, 4, 4> proj;
    matrix<float, 4, 4> view;
    matrix<float, 4, 4> model;
};

struct VSInput
{
    [[vk::location(0)]]  
    float3 position : POSITION0;
    [[vk::location(1)]]   
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    [[vk::location(0)]]
    float4 position : SV_POSITION;
    [[vk::location(1)]]
    float2 uv : TEXCOORD0;
};

VSOutput main( in VSInput input ) 
{
    VSOutput output = (VSOutput) 0;
    output.position = mul(proj, mul(view, mul(model, float4(input.position, 1.0f))));
    output.uv = input.uv;
	return output;
}