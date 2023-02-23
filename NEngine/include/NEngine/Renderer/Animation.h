#pragma once
#include <string>
#include <vector>

#include "NEngine/Math/NEMath.h"
#include "RenderNode.h"

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
    int target_node = 0;
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
    std::vector<float> InterpolateWeights(const Channel &ch,
                                          int len,
                                          float t,
                                          float max_time);

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

    void Advance(float dt, std::vector<RenderNode> &render_nodes);
};
}  // namespace NEngine::Renderer
