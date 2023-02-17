#pragma once
#include <string>
#include <vector>

#include "NEngine/Helpers/Transform.h"
#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/NEMath.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Math;

namespace NEngine::Renderer {
struct AnimSampler
{
    std::vector<float> input;
    std::vector<unsigned char> output;
    size_t output_stride;
};

struct Channel
{
    enum class Path {
        Translation,
        Rotation,
        Scale,
        Weights,
        Invalid
    };
    Path target_path = Path::Invalid;
    size_t target_node = 0;
    AnimSampler sampler;
};

struct Interpolator
{
    size_t prev_key = 0;
    size_t next_key = 0;
    float prev_t = 0.0f;
    float next_t = 0.0f;
    vec3 InterpolateVec3(const Channel &ch, float t, float max_time);
    vec4 InterpolateVec4(const Channel &ch, float t, float max_time);

private:
    float Interpolate(const Channel &ch, float t, float max_time);
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
    std::vector<Interpolator> interpolators;
    float max_time = 0.0f;
    float cur_time = 0.0f;

    void
    Advance(float dt, Helpers::Transform &transform)
    {
        cur_time += dt / 1000.0f;

        if (NearlyEqual(0.0f, max_time)) {
            for (const auto &ch : channels) {
                for (const auto &t : ch.sampler.input) {
                    max_time = std::max(max_time, t);
                }
            }
        }

        for (size_t i = 0; i < interpolators.size(); ++i) {
            const auto &ch = channels[i];
            auto &interpolator = interpolators[i];

            switch (ch.target_path) {
                case Channel::Path::Translation:
                {
                    const auto t =
                        interpolator.InterpolateVec3(ch, cur_time, max_time);
                    transform.translation = t;
                } break;
                case Channel::Path::Rotation:
                {
                    const auto r =
                        interpolator.InterpolateVec4(ch, cur_time, max_time);
                    transform.rotation = r;
                } break;
                case Channel::Path::Scale:
                {
                    const auto s =
                        interpolator.InterpolateVec3(ch, cur_time, max_time);
                    transform.scale = s;
                } break;
                case Channel::Path::Weights:
                    break;
                default:
                    UTILS_ASSERT(false, "Invalid path");
            }
        }
    }
};
}  // namespace NEngine::Renderer
