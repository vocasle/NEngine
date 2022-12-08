#include "BasePass.hlsli"
#include "PBR/PBR.hlsli"
#include "PBR/ToneMapping.hlsli"

struct MaterialInfo {
  float ior;
  float perceptualRoughness; // roughness value, as authored by the model
                             // creator (input to shader)
  float3 f0;                 // full reflectance color (n incidence angle)

  float alphaRoughness; // roughness mapped to a more linear change in the
                        // roughness (proposed by [2])
  float3 c_diff;

  float3 f90; // reflectance color at grazing angle
  float metallic;

  float3 baseColor;

  float sheenRoughnessFactor;
  float3 sheenColorFactor;

  float3 clearcoatF0;
  float3 clearcoatF90;
  float clearcoatFactor;
  float3 clearcoatNormal;
  float clearcoatRoughness;

  // KHR_materials_specular
  float specularWeight; // product of specularFactor and specularTexture.a

  float transmissionFactor;

  float thickness;
  float3 attenuationColor;
  float attenuationDistance;

  // KHR_materials_iridescence
  float iridescenceFactor;
  float iridescenceIor;
  float iridescenceThickness;
};

// KHR_lights_punctual extension.
// see
// https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual
struct Light {
  float3 direction;
  float range;

  float3 color;
  float intensity;

  float3 position;
  float innerConeCos;

  float outerConeCos;
  int type;
};

static const int LightType_Directional = 0;
static const int LightType_Point = 1;
static const int LightType_Spot = 2;

// Calculates a halfway vector H
// L - direction to light source
// V - direction to view
float3 HalfwayVec(float3 L, float3 V) { return normalize((L + V)); }

// Return tangent space normal transformed into world coordinates
// N - vertex normal in world
// T - tangent in world
// B - bitangent in world
float3 GetNormal(PSIn pin) {
  float4 normSampled = normalTex.Sample(normalSam, pin.TexCoords);
  float3 vT = normalize(pin.TangentW);
  float3 vN = normalize(pin.NormalW);
  float3 vB = normalize(pin.BitangentW);
  float3 vNt = normSampled.rgb * 2.0 - 1.0;
  vNt *= float3(material.NormalScale, material.NormalScale, 1.0);
  vNt = normalize(vNt);
  float3x3 TBN = float3x3(vT, vB, vN);
  return mul(transpose(TBN), vNt);
}

float clampedDot(float3 x, float3 y) { return clamp(dot(x, y), 0.0, 1.0); }

// TODO: Pass vertex colors to shader
float4 GetVertexColor(PSIn pin) { return float4(1, 1, 1, 1); }

float4 GetBaseColor(PSIn pin) {
  float4 baseColor = float4(1, 1, 1, 1);
  baseColor = material.BaseColor;
  baseColor *= baseColorTex.Sample(baseColorSam, pin.TexCoords);

  return baseColor * GetVertexColor(pin);
}

MaterialInfo GetMetallicRoughnessInfo(PSIn pin, MaterialInfo info) {
  info.metallic = material.Metalness;
  info.perceptualRoughness = material.Roughness;

  // Roughness is stored in the 'g' channel, metallic is stored in the 'b'
  // channel. This layout intentionally reserves the 'r' channel for (optional)
  // occlusion map data
  float4 mrSample =
      metallicRoughnessTex.Sample(metallicRoughnessSam, pin.TexCoords);
  info.perceptualRoughness *= mrSample.g;
  info.metallic *= mrSample.b;

  // Achromatic f0 based on IOR.
  info.c_diff = lerp(info.baseColor.rgb, float3(0, 0, 0), info.metallic);
  info.f0 = lerp(info.f0, info.baseColor.rgb, info.metallic);
  return info;
}

float3 GetLighIntensity(Light light, float3 pointToLight) {
  float rangeAttenuation = 1.0;
  float spotAttenuation = 1.0;

  if (light.type != LightType_Directional) {
    // rangeAttenuation = getRangeAttenuation(light.range,
    // length(pointToLight));
  }
  if (light.type == LightType_Spot) {
    // spotAttenuation = getSpotAttenuation(pointToLight, light.direction,
    // light.outerConeCos, light.innerConeCos);
  }

  return rangeAttenuation * spotAttenuation * light.intensity * light.color;
}

float4 main(PSIn pin) : SV_TARGET {

  float4 baseColor = GetBaseColor(pin);
  float3 V = normalize(cameraPosW - pin.PosW);

  MaterialInfo materialInfo;
  materialInfo.baseColor = baseColor.rgb;
  // The default index of refraction of 1.5 yields a dielectric normal incidence
  // reflectance of 0.04.
  materialInfo.ior = 1.5;
  materialInfo.f0 = float3(0.04, 0.04, 0.04);
  materialInfo.specularWeight = 1.0;

  materialInfo = GetMetallicRoughnessInfo(pin, materialInfo);

  materialInfo.perceptualRoughness =
      clamp(materialInfo.perceptualRoughness, 0.0, 1.0);
  materialInfo.metallic = clamp(materialInfo.metallic, 0.0, 1.0);

  // Roughness is authored as perceptual roughness; as is convention,
  // convert to material roughness by squaring the perceptual roughness.
  materialInfo.alphaRoughness =
      materialInfo.perceptualRoughness * materialInfo.perceptualRoughness;

  // Compute reflectance.
  float reflectance =
      max(max(materialInfo.f0.r, materialInfo.f0.g), materialInfo.f0.b);

  // Anything less than 2% is physically impossible and is instead considered to
  // be shadowing. Compare to "Real-Time-Rendering" 4th editon on page 325.
  materialInfo.f90 = float3(1.0, 1.0, 1.0);

  // LIGHTING
  float3 f_specular = float3(0.0, 0.0, 0.0);
  float3 f_diffuse = float3(0.0, 0.0, 0.0);
  float3 f_emissive = float3(0.0, 0.0, 0.0);
  float3 f_clearcoat = float3(0.0, 0.0, 0.0);
  float3 f_sheen = float3(0.0, 0.0, 0.0);
  float albedoSheenScaling = 1.0;

  float ao = 1.0;
  // Apply optional PBR terms for additional (optional) shading
  ao = occlusionTex.Sample(occlusionSam, pin.TexCoords).r;
  f_diffuse = lerp(f_diffuse, f_diffuse * ao, material.OcclusionStrength);
  // apply ambient occlusion to all lighting that is not punctual
  f_specular = lerp(f_specular, f_specular * ao, material.OcclusionStrength);

  // TODO: Do this for each light
  {
    float3 pointToLight = -dirLight.Direction.xyz;
    // BSTF
    float3 L = -dirLight.Direction.xyz;
    float3 H = HalfwayVec(L, V);
    float3 N = GetNormal(pin);
    float NdotV = clampedDot(N, V);
    float NdotL = clampedDot(N, L);
    float VdotH = clampedDot(V, H);
    float NdotH = clampedDot(N, H);
    Light light;
    light.intensity = 1.0;
    light.color = float3(1.0, 1.0, 1.0);

    if (NdotL > 0.0 || NdotV > 0.0) {
      float3 intensity = GetLighIntensity(light, pointToLight);

      f_diffuse += intensity * NdotL *
                   BRDF_lambertian(materialInfo.f0, materialInfo.f90,
                                   materialInfo.c_diff,
                                   materialInfo.specularWeight, VdotH);
      f_specular += intensity * NdotL *
                    BRDF_specularGGX(materialInfo.f0, materialInfo.f90,
                                     materialInfo.alphaRoughness,
                                     materialInfo.specularWeight, VdotH, NdotL,
                                     NdotV, NdotH);
    }
  }

  f_emissive = material.EmissiveFactor;
  f_emissive *= emissiveTex.Sample(emissiveSam, pin.TexCoords).rgb;

  // Layer blending
  float clearcoatFactor = 0.0;
  float3 clearcoatFresnel = (float3)0;
  float3 color = (float3)0;
  float3 diffuse = f_diffuse;
  color = f_emissive + diffuse + f_specular;
  color = f_sheen + color * albedoSheenScaling;
  color = color * (1.0 - clearcoatFactor * clearcoatFresnel) + f_clearcoat;

  return float4(toneMap(color.rgb), baseColor.a);
}