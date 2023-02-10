#pragma once
#include <string>
#include <vector>

namespace NEngine::Renderer {
struct AnimSampler
{
    std::vector<unsigned char> inputs;
    std::vector<unsigned char> outputs;
    int input_type;
    int output_type;
};

struct Channel
{
    AnimSampler sampler;
    std::string path;
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
};
}  // namespace NEngine::Renderer
