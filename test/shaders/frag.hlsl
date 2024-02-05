#ifdef KHR
#define LOCATION(x) [[vk::location(x)]]
#else 
#define LOCATION(x)
#endif

struct VSInput
{
    LOCATION(0)
    float3 position : POSITION0;
    LOCATION(1) 
    float2 uv : TEXCOORD0;
};

float4 main(in VSInput input) : SV_TARGET
{
    return float4(input.uv.r, input.uv.g, 0.0f, 1.0f);
}