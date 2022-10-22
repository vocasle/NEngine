#include "Common.hlsli"

static const float AMBIENT = 0.1f;
static const float PRECISION = 0.000001f;

float4 main(VSOut In) : SV_TARGET
{
    const float2 uv = float2(In.PosW.w, In.NormalW.w);
    const float4 diffuseSampled = diffuseTexture.Sample(defaultSampler, uv);
    const float4 specularSampled = specularTexture.Sample(defaultSampler, uv);
    const float4 glossSampled = glossTexture.Sample(defaultSampler, uv);
    const float3 normalSampled = normalTexture.Sample(defaultSampler, uv).xyz;
    const float3 normal = NormalSampleToWorldSpace(normalSampled, normalize(In.NormalW.xyz), normalize(In.TangentW));
    const float3 viewDir = normalize(cameraPosW - In.PosW.xyz);
    const float2 shadowUV = float2(In.ShadowPosH.x * 0.5f + 0.5f,
                                                   -In.ShadowPosH.y * 0.5f + 0.5f);
    const float4 shadowSampled = shadowTexture.Sample(defaultSampler, shadowUV);

    float3 diffuseResult = {0,0,0};
    float3 specularResult = {0,0,0};

    LightIntensity intensities[8];

    intensities[0] = DirectionalLightIntensity(dirLight, normal, viewDir);
    intensities[1] = PointLightIntensity(pointLights[0], normal, In.PosW.xyz, viewDir);
    intensities[2] = PointLightIntensity(pointLights[1], normal, In.PosW.xyz, viewDir);
    intensities[3] = PointLightIntensity(pointLights[2], normal, In.PosW.xyz, viewDir);
    intensities[4] = PointLightIntensity(pointLights[3], normal, In.PosW.xyz, viewDir);
    intensities[5] = PointLightIntensity(pointLights[4], normal, In.PosW.xyz, viewDir);
    intensities[6] = PointLightIntensity(pointLights[5], normal, In.PosW.xyz, viewDir);
    intensities[7] = SpotLightIntensity(spotLights[0], normal, In.PosW.xyz, viewDir);

    const float strength = dot(diffuseResult, specularResult);
    float shadow = 1;
    static const float dx = 1.0 / 2048.0;
    const float2 texelSize = {dx, dx};
    if (In.ShadowPosH.z > shadowSampled.x)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x) {
            [unroll]
            for (int y = -1; y <= 1; ++y) {
//                const float pcfDepth = shadowTexture.Sample(defaultSampler, shadowUV + float2(x,y) * texelSize).r;
//                const float pcfDepth = shadowTexture.SampleCmpLevelZero(shadowSampler, shadowUV + float2(x,y) * texelSize, In.ShadowPosH.z).r;
                const float pcfDepth = CalcShadowFactor(shadowSampler, shadowTexture, In.ShadowPosH);
                shadow += pcfDepth;
            }
        }
        shadow /= 9.0;
    }

    float3 lightDiffuseResult = {1,1,1};
    [unroll]
    for (int i = 0; i < 8; ++i) {
        float diff = max(dot(intensities[i].L, normal), 0);
        float3 diffuseColor = diffuseSampled.rgb * diff;
        float spec = pow(max(dot(normal, intensities[i].H), 0), 16);
        float3 specularColor = glossSampled.bbb * spec;

        // TODO: Find out how to affect the area near the light source with light color? May be via Bloom?
        diffuseResult += diffuseColor * intensities[i].intensity;
        specularResult += specularColor * intensities[i].intensity;
        lightDiffuseResult *= intensities[i].diffuse;
    }

//    return float4(float3(0.5,0.5,0.5) * intensities[0].intensity, 1); diffuse shading for debug purposes
    float4 reflColor = (float4)0;
    if (calcReflection) {
        float3 incident = -viewDir;
    	float3 reflVector = normalize(reflect(incident, normal));
    	reflColor = envTexture.Sample(defaultSampler, reflVector);
    }

    float4 fragmentColor = float4(AMBIENT * lightDiffuseResult + shadow * (diffuseResult + specularResult), 1);
    fragmentColor += reflColor * (1.0 - material.roughness);
    return fragmentColor;
}
