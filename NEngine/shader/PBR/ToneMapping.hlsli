static const float GAMMA = 2.2;
static const float INV_GAMMA = 1.0 / GAMMA;

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
    float3x3(0.59719, 0.07600, 0.02840, 0.35458, 0.90834, 0.13383, 0.04823,
             0.01566, 0.83777);

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
    float3x3(1.60475, -0.10208, -0.00327, -0.53108, 1.10813, -0.07276, -0.07367,
             -0.00605, 1.07602);

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
float3 linearTosRGB(float3 color) {
  return pow(color, float3(INV_GAMMA, INV_GAMMA, INV_GAMMA));
}

float3 toneMap(float3 color) {
  float exposure = 1.0;
  color *= exposure;

#ifdef TONEMAP_ACES_NARKOWICZ
  color = toneMapACES_Narkowicz(color);
#endif

#ifdef TONEMAP_ACES_HILL
  color = toneMapACES_Hill(color);
#endif

#ifdef TONEMAP_ACES_HILL_EXPOSURE_BOOST
  // boost exposure as discussed in
  // https://github.com/mrdoob/three.js/pull/19621 this factor is based on the
  // exposure correction of Krzysztof Narkowicz in his implemetation of ACES
  // tone mapping
  color /= 0.6;
  color = toneMapACES_Hill(color);
#endif

  return linearTosRGB(color);
}