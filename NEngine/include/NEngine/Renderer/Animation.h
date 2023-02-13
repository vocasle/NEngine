#pragma once
#include <string>
#include <vector>

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/NEMath.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Math;

namespace NEngine::Renderer {
struct AnimSampler
{
    std::vector<float> inputs;
    std::vector<unsigned char> outputs;
    int input_stride;
    int output_stride;
};

struct Channel
{
    AnimSampler sampler;
    std::string path;
};

struct Interpolator
{
    size_t prev_key = 0;
    float prev_time = 0;
    float anim_time = 0;

    vec3
    InterpolateScale(const Channel &ch, float t)
    {
        return InterpolateVec3(ch, t);
    }

    vec3
    InterpolateTranslation(const Channel &ch, float t)
    {
        return InterpolateVec3(ch, t);
    }

    vec4
    InterpolateRotation(const Channel &ch, float t)
    {
        size_t next_key = 0;
        const float tn = Interpolate<vec4>(ch, t, next_key);

        const auto &outputs =
            *reinterpret_cast<const std::vector<vec4> *>(&ch.sampler.outputs);

        const auto q0 = outputs[prev_key];
        const auto q1 = outputs[next_key];
        return QuatSlerp(q0, q1, tn);
    }

private:
    vec3
    InterpolateVec3(const Channel &ch, float t)
    {
        size_t next_key = 0;
        const float tn = Interpolate<vec3>(ch, t, next_key);

        const auto &outputs =
            *reinterpret_cast<const std::vector<vec3> *>(&ch.sampler.outputs);

        const auto v0 = outputs[prev_key];
        const auto v1 = outputs[next_key];
        return Lerp(v0, v1, tn);
    }

    template <typename T>
    float
    Interpolate(const Channel &ch, float t, size_t &next_key)
    {
        anim_time += t / 1000.0f; // convert to seconds

        // Wrap t around, so the animation loops.
        // Make sure that t is never earlier than the first keyframe and never
        // later then the last keyframe.
        const auto &inputs = ch.sampler.inputs;
        t = fmodf(anim_time, inputs[inputs.size() - 1]);
        // t = Clamp(t, inputs[0], inputs[inputs.size() - 1]);

        if (prev_time > t) {
            prev_key = 0;
        }

        prev_time = t;

        const auto &outputs =
            *reinterpret_cast<const std::vector<T> *>(&ch.sampler.outputs);
        for (size_t i = prev_key; i < outputs.size(); ++i) {
            if (t <= inputs[i]) {
                next_key = Clamp(i, size_t(1), inputs.size() - 1);
                break;
            }
        }

        const float key_delta = inputs[next_key] - inputs[prev_key];
        // Normalize t: [t0, t1] -> [0, 1]
        const float tn = (t - inputs[prev_key]) / key_delta;

        return tn;
    }
};

struct Animation
{
    enum class Interpolation {
        Step,
        Linear,
        Cubicspline
    };

    std::string name;
    std::vector<Channel> channels;
    Interpolator interpolator;
};
}  // namespace NEngine::Renderer
