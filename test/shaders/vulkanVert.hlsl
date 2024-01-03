float3 positions[3] =
{
	   float3( 0.0f, -0.5f, 0.0f ),
	   float3( 0.5f,  0.5f, 0.0f ),
	   float3(-0.5f,  0.5f, 0.0f )
};

void main(float4 position : SV_POSITION, uint VertexIndex : SV_VertexID)
{
    position = float4(positions[VertexIndex], 1.0f);
}