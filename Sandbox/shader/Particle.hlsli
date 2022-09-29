struct PSIn
{
	float4 PosH : SV_POSITION;
	float2 TexCoords : TEXCOORDS;
	float Lifespan : LIFESPAN;
};

struct VSIn
{
	float3 Pos : POSITION;
	float2 TexCoords : TEXCOORDS;
	float Lifespan : LIFESPAN;
};

Texture2D<float4> diffuseTexture : register(t0);
sampler defaultSampler : register(s0);

cbuffer PerFrameConstants : register(b0)
{
	float4x4 view;
	float4x4 proj;
	float4 color;
};
