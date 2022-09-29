static const float4 ZERO_VEC4 = { 0.0f, 0.0f, 0.0f, 0.0f };
static const float4 ONE_VEC4 = { 1.0f, 1.0f, 1.0f, 1.0f };
static const uint MAX_LIGHTS = 8;

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
	float roughness;
};

struct LightIntensity
{
    float3 intensity;
    float3 L;
    float3 H;
    float3 diffuse;
};

struct Color
{
	float4 Emissive;
	float4 Diffuse;
	float4 Specular;
};

LightIntensity DirectionalLightIntensity(DirectionalLight light, float3 normal, float3 viewDir)
{
    LightIntensity intensity;
    intensity.intensity = float3(1.0f, 1.0f, 1.0f);
    const float3 L = normalize(light.Position.xyz);
    const float3 H = normalize(L + viewDir);
    intensity.L = L;
    intensity.H = H;
    intensity.diffuse = light.Diffuse.rgb;
    return intensity;
}

LightIntensity PointLightIntensity(PointLight light, float3 normal, float3 surfPoint, float3 viewDir)
{
    LightIntensity intensity;
    const float distance = length(light.Position.xyz - surfPoint);
    const float atten = light.Position.w / (distance * distance);
    intensity.intensity = float3(atten, atten, atten);
    const float3 L = normalize(light.Position.xyz - surfPoint);
    const float3 H = normalize(L + viewDir);
    intensity.L = L;
    intensity.H = H;
    intensity.diffuse = light.Diffuse.rgb;
    return intensity;
}

LightIntensity SpotLightIntensity(SpotLight light, float3 normal, float3 surfPoint, float3 viewDir)
{
    LightIntensity intensity;
    const float distance = length(light.Position.xyz - surfPoint);
    const float3 L = normalize(light.Position.xyz - surfPoint);
    const float atten = pow(max(dot(-light.Direction.xyz, L), 0.0f), light.Direction.w) / (distance * distance);
    intensity.intensity = float3(atten, atten, atten);
    const float3 H = normalize(L + viewDir);
    intensity.L = L;
    intensity.H = H;
    intensity.diffuse = light.Diffuse.rgb;
    return intensity;
}

// Blinn-Phong
// K = EM + DTA + Sum {Ci[DT(dot(N,Li) + SG(dot(N,Hi)^m]}
// E - emission color
// M - sampled color from emission map
// D - surface's diffuse reflection color
// T - sampled color from diffuse map
// A - ambient intensity
// Ci - light intensity
// N - normal in world space
// Li - unit vector that point from surface point towards i-th light source
// S - surface's specular color
// G - sampled color from gloss map
// Hi - halfway vector of i-th light source, i.e. H = Li + V / ||Li + V||, where V is unit direction vector to viewer from surface point
// m - specular exponent
Color BlinnPhong(float4 E,
    float4 M,
    float4 D,
    float4 T,
    float4 A,
    float4 S,
    float4 G,
    float m,
    float3 N,
    LightIntensity intensities[MAX_LIGHTS],
    uint numLights)
{
    Color outCol;
    float4 sum = ZERO_VEC4;
    const float4 EM = E * M;
    const float4 DT = D * T;
    const float4 SG = float4(S.rgb, 1.0f) * G;
    [unroll]
    for (uint i = 0; i < numLights; ++i)
    {
        const float3 Ci = intensities[i].intensity;
        const float3 Li = normalize(intensities[i].L);
        const float3 Hi = normalize(intensities[i].H);
        const float3 lightDiffuse = intensities[i].diffuse;
        sum += float4(Ci, 1.0f) * (DT * float4(lightDiffuse, 1.0f) * max(dot(N, Li), 0.0f) + SG * pow(max(dot(N, Hi), 0.0f), m));
    }
    outCol.Emissive = EM;
    outCol.Diffuse = DT * A;
    outCol.Specular = sum;
    //return EM + DT * A + sum;
    return outCol;
}

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
    Texture2D shadowMap,
    float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    // Texel size.
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    float2 shadowUV = float2(shadowPosH.x * 0.5f + 0.5f,
        -shadowPosH.y * 0.5f + 0.5f);

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            shadowUV + offsets[i], depth).r;
    }

    return percentLit /= 9.0f;
}

//---------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample,
    float3 unitNormalW,
    float4 tangentW)
{
    // Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMapSample - 1.0f;
    // Build orthonormal basis.
    float3 N = normalize(unitNormalW);
    float3 T = normalize(tangentW.xyz - dot(tangentW.xyz, N) * N);
    float3 B = cross(N, T) * tangentW.w;
    float3x3 TBN = float3x3(T, B, N);
    // Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);
    return bumpedNormalW;
}
