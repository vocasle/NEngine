#include "BasePass.hlsli"

// Taken from: http://www.thetenthplanet.de/archives/1180
float3x3 cotangent_frame(float3 N, float3 p, float2 uv) {
  // get edge vectors of the pixel triangle
  float3 dp1 = ddx(p);
  float3 dp2 = ddy(p);
  float2 duv1 = ddx(uv);
  float2 duv2 = ddy(uv);
  // solve the linear system
  float3 dp2perp = cross(dp2, N);
  float3 dp1perp = cross(N, dp1);
  float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
  float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
  // construct a scale-invariant frame
  float invmax = rsqrt(max(dot(T, T), dot(B, B)));
  return float3x3(T * invmax, B * invmax, N);
}

//
// lambert lighting function
//
float3 LambertLighting(float3 lightNormal, float3 surfaceNormal,
                       float3 lightColor, float3 pixelColor) {
  // compute amount of contribution per light
  float diffuseAmount = saturate(dot(lightNormal, surfaceNormal));
  float3 diffuse = diffuseAmount * lightColor * pixelColor;
  return diffuse;
}

//
// specular contribution function
//
float3 SpecularContribution(float3 toEye, float3 lightNormal,
                            float3 surfaceNormal, float3 materialSpecularColor,
                            float materialSpecularPower,
                            float lightSpecularIntensity, float3 lightColor) {
  // compute specular contribution
  float3 vHalf = normalize(lightNormal + toEye);
  float specularAmount = saturate(dot(surfaceNormal, vHalf));
  specularAmount = pow(specularAmount, max(materialSpecularPower, 0.0001f)) *
                   lightSpecularIntensity;
  float3 specular = materialSpecularColor * lightColor * specularAmount;

  return specular;
}

float4 main(PSIn pin) : SV_TARGET {
#if CALC_TBN_ON_THE_FLY
  float3 N = normalize(pin.NormalW);
  float3 toEye = cameraPosW - pin.PosW;
  float3x3 TBN = cotangent_frame(N, -toEye, pin.TexCoords);
  float3 Nt = normalTex.Sample(normalSam, pin.TexCoords).xyz * 2.0 - 1.0;
  Nt *= float3(material.NormalScale, material.NormalScale, 1);
  Nt = normalize(Nt);
  float3 vNout = mul(transpose(TBN), Nt);
#else
  float4 normSampled = normalTex.Sample(normalSam, pin.TexCoords);
  float3 vT = normalize(pin.TangentW);
  float3 vN = normalize(pin.NormalW);
  float3 vB = normalize(pin.BitangentW);
  float3 vNt = normSampled.rgb * 2.0 - 1.0;
  vNt *= float3(material.NormalScale, material.NormalScale, 1.0);
  vNt = normalize(vNt);
  float3x3 TBN = float3x3(vT, vB, vN);
  float3 vNout = mul(transpose(TBN), vNt);
#endif

  float4 baseColor =
      material.BaseColor * baseColorTex.Sample(baseColorSam, pin.TexCoords);

  float3 local1 = 0;
  float3 local4 = 0;

  float3 toEye = normalize(cameraPosW - pin.PosW);
  float4 metallicRoughness =
      metallicRoughnessTex.Sample(metallicRoughnessSam, pin.TexCoords);

  local1 += LambertLighting(dirLight.Direction.xyz, vNout, dirLight.Diffuse.rgb,
                            baseColor.rgb);

  float specular = 1 - metallicRoughness.b;
  local4 += SpecularContribution(toEye, dirLight.Direction.xyz, vNout,
                                 float3(specular, specular, specular), 16, 1,
                                 dirLight.Specular.rgb);

  local1 = saturate(local1);

  return float4(local1 + local4, baseColor.a);
}
