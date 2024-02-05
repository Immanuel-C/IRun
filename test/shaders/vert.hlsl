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

struct VSOutput
{
    LOCATION(0)
    float4 position : SV_POSITION;
    LOCATION(1)
    float2 uv : TEXCOORD0;
};

VSOutput main( in VSInput input ) 
{
    VSOutput output = (VSOutput) 0;
    output.position = float4(input.position, 1.0f);
    output.uv = input.uv;
	return output;
}