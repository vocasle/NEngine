#include "NEngine/Renderer/Animation.h"

namespace NEngine::Renderer {

vec3
Interpolator::InterpolateVec3(const Channel &ch, float t, float max_time)
{
    const auto tn = Interpolate(ch, t, max_time);
    const auto output =
        *reinterpret_cast<const std::vector<vec3> *>(&ch.sampler.output);
    UTILS_PRINTLN(
        "t: %.4f, prev_key: %llu, next_key: %llu", t, prev_key, next_key);

    return Lerp(output[prev_key], output[next_key], tn);
}

vec4
Interpolator::InterpolateVec4(const Channel &ch, float t, float max_time)
{
    const auto tn = Interpolate(ch, t, max_time);
    const auto output =
        *reinterpret_cast<const std::vector<vec4> *>(&ch.sampler.output);
    UTILS_PRINTLN(
        "t: %.4f, prev_key: %llu, next_key: %llu", t, prev_key, next_key);

    return QuatSlerp(output[prev_key], output[next_key], tn);
}

float
Interpolator::Interpolate(const Channel &ch, float t, float max_time)
{
    const auto input = ch.sampler.input;

    t = fmodf(t, max_time);
    t = Clamp(t, input[0], input[input.size() - 1]);

    if (prev_t > t) {
        prev_key = 0;
    }

    prev_t = t;

    for (size_t i = prev_key; i < input.size(); ++i) {
        if (t < input[i]) {
            next_key = Clamp(i, 1LLU, input.size() - 1LLU);
            break;
        }
    }

    prev_key = Clamp(next_key - 1LLU, 0LLU, next_key);
    const auto key_delta = input[next_key] - input[prev_key];
    const auto tn = (t - input[prev_key]) / key_delta;
    return tn;
}
}  // namespace NEngine::Renderer
