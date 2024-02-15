struct VSInput
{
    [[vk::location(0)]]
    float3 position : POSITION0;
    [[vk::location(1)]] 
    float2 uv : TEXCOORD0;
};

float4 main(in VSInput input) : SV_TARGET
{
    return float4(input.uv.r, input.uv.g, 0.0f, 1.0f);
}