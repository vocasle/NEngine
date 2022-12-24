#include "BasePass.hlsli"

static const float M_PI = 3.141592653589793;

// This is my implementation of formulas found on
// https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// with help from
// https://learnopengl.com/PBR/Theory

// dot(N, V) could not be negative, thus we either clamp it to zero
// or use abs value + 0.00001

// Return tangent space normal transformed into world coordinates
// N - vertex normal in world
// T - tangent in world
// B - bitangent in world
float3
GetNormal(PSIn pin)
{
    float3 normal = normalize(pin.NormalW);
    if (material.HasNormalTex) {
        float4 normSampled = normalTex.Sample(normalSam, pin.TexCoords);
        float3 vT = normalize(pin.TangentW);
        float3 vN = normal;
        float3 vB = normalize(pin.BitangentW);
        float3 vNt = normSampled.rgb * 2.0 - 1.0;
        vNt *= float3(material.NormalScale, material.NormalScale, 1.0);
        vNt = normalize(vNt);
        float3x3 TBN = float3x3(vT, vB, vN);
        normal = mul(transpose(TBN), vNt);
    }
    return normal;
}

float
ClampedDot(float3 V1, float3 V2)
{
    return clamp(dot(V1, V2), 0.0, 1.0);
}

// The following equation(s) model the distribution of microfacet normals across
// the area being drawn (aka D()) Implementation from "Average Irregularity
// Representation of a Roughened Surface for Ray Reflection" by T. S.
// Trowbridge, and K. P. Reitz Follows the distribution function recommended in
// the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float
D_GGX(float NdotH, float alphaRoughness)
{
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;
    float f = (NdotH * NdotH) * (alphaRoughnessSq - 1.0) + 1.0;
    return alphaRoughnessSq / (M_PI * f * f);
}

// Smith Joint GGX
// Note: Vis = G / (4 * NdotL * NdotV)
// see Eric Heitz. 2014. Understanding the Masking-Shadowing Function in
// Microfacet-Based BRDFs. Journal of Computer Graphics Techniques, 3 see
// Real-Time Rendering. Page 331 to 336. see
// https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
float
V_GGX(float NdotL, float NdotV, float alphaRoughness)
{
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;
    float GGXV = NdotV / (NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float GGXL = NdotL / (NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    return GGXV * GGXL;
}

float3
F_Schlick(float3 F0, float NdotV)
{
    return F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
}

float3
BRDF_lambertian(float3 f0,
                float3 diffuseColor,
                float specularWeight,
                float NdotV)
{
    // see
    // https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    return (1.0 - specularWeight * F_Schlick(f0, NdotV)) *
           (diffuseColor / M_PI);
}

float3
BRDF_specularGGX(float3 f0,
                 float alphaRoughness,
                 float specularWeight,
                 float NdotL,
                 float NdotV,
                 float NdotH)
{
    float3 F = F_Schlick(f0, NdotV);
    float Vis = V_GGX(NdotL, NdotV, alphaRoughness);
    float D = D_GGX(NdotH, alphaRoughness);

    return specularWeight * F * Vis * D;
}

float3
Chessboard(PSIn pin)
{
    float x = floor(pin.PosW.x) + floor(pin.PosW.z);
    x = frac(x * 0.5);
    x *= 2.0;

    float res = x;

    return res.xxx;
}

float4
main(PSIn pin)
    : SV_TARGET
{
    // For directional light
    // L - direction from point on surface to the light
    // V - direction from point to view
    // Clight - color of the light
    // Lo(V) = M_PI * BRDF(L, V) Clight * clamped_dot(N, L)

    // For point and spot lights Clight falls of as the inverse
    // square of the distance to the light

    // For punctual lights
    // Lo(V) = M_PI * sum (BRDF(L_i, V) Clight_i * clamped_dot(N, L_i)), from i
    // = 1 to n n - number of lights in the scene

    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 N = GetNormal(pin);
    float3 L = -normalize(dirLight.Direction.xyz);
    float3 V = normalize(cameraPosW - pin.PosW);
    float3 H = normalize(V + L);
    float NdotL = saturate(dot(N, L));
    float NdotH = saturate(dot(N, H));
    float NdotV = saturate(dot(N, V));
    float HdotV = saturate(dot(H, V));

    float4 baseColor = material.BaseColor;

    if (material.HasBaseColorTex) {
        baseColor *= baseColorTex.Sample(baseColorSam, pin.TexCoords);
    }
    else {
        baseColor.rgb += Chessboard(pin);
    }

    float roughness = 1.0;
    float metallic = 0.0;

    if (material.HasMetallicRoughnessTex) {
        float4 metallicRoughness =
            metallicRoughnessTex.Sample(metallicRoughnessSam, pin.TexCoords);

        roughness = metallicRoughness.g;
        metallic = metallicRoughness.b;
    }

    float alphaRoughness = roughness * roughness;
    float specularWeight = 1.0;

    F0 = lerp(F0, baseColor.rgb, metallic);

    float3 f_diffuse =
        BRDF_lambertian(F0, baseColor.rgb, specularWeight, NdotV);
    float3 f_specular = BRDF_specularGGX(
        F0, alphaRoughness, specularWeight, NdotL, NdotV, NdotH);

#ifdef DEBUG_NDF
    float D = D_GGX(NdotH, alphaRoughness);
    return float4(D.xxx, 1.0);
#endif
#ifdef DEBUG_GEOMETRY
    float G = V_GGX(NdotL, NdotV, alphaRoughness);
    return float4(G.xxx, 1.0);
#endif
#ifdef DEBUG_FRESNEL
    float3 F = F_Schlick(F0, NdotV);
    return float4(F, 1.0);
#endif
#ifdef DEBUG_NORMAL
    return float4(N, 1.0);
#endif
#ifdef DEBUG_ROUGHNESS
    return float4(roughness.xxx, 1.0);
#endif
#ifdef DEBUG_METALLIC
    return float4(metallic.xxx, 1.0);
#endif
#ifdef DEBUG_BASE_COLOR
    return float4(baseColor.rgb, 1.0);
#endif
#ifdef DEBUG_DIFFUSE_BRDF
    return float4(f_diffuse, 1.0);
#endif
#ifdef DEBUG_SPECULAR_BRDF
    return float4(f_specular, 1.0);
#endif

    return float4(f_diffuse + f_specular, baseColor.a);
}