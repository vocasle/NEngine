// Types

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Position; // W - is radius
};

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Position; // W - is range
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Position; // W - is range
	float4 Direction; // W - is spot
};

struct Material
{
	float4 BaseColor;
    float Metalness;
    float Roughness;
};

// Shader inputs/outputs

struct VSIn
{
	float4 Pos      : POSITION;
	float4 Normal   : NORMAL;
	float4 Tangent  : TANGENT;
};

struct PSIn
{
	float4 PosH     : SV_POSITION;
	float4 NormalW  : NORMAL;
	float4 PosW     : POSITION;
	float4 TangentW : TANGENT;
};

// Constant buffers

cbuffer PerObjectConstants : register(b0)
{
	float4x4 worldInvTranspose;
	float4x4 world;
	float4x4 shadowTransform;
	Material material;
};

cbuffer PerFrameConstants : register(b1)
{
	float4x4 view;
	float4x4 proj;
	float3 cameraPosW;
};

cbuffer PerSceneConstants : register(b2)
{
	DirectionalLight dirLight;
	PointLight pointLights[6];
	SpotLight spotLights[2];
};

// Samplers

sampler baseColorSam            : register(s0);
sampler metallicRoughnessSam    : register(s1);
sampler normalSam				: register(s2);
sampler occlusionSam			: register(s3);
sampler emissiveSam				: register(s4);

// Textures

Texture2D<float4> baseColorTex	        : register(t0);
Texture2D<float4> metallicRoughnessTex	: register(t1);
Texture2D<float4> normalTex		        : register(t2);
Texture2D<float4> occlusionTex		    : register(t3);
Texture2D<float4> emissiveTex		    : register(t4);
