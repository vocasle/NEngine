#include "NEngine/Renderer/Animation.h"

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/NEMath.h"

namespace NEngine::Renderer {

vec3
Interpolator::InterpolateVec3(const Channel &ch, float t, float max_time)
{
    const auto tn = Interpolate(ch, t, max_time);
    const auto output =
        *reinterpret_cast<const std::vector<vec3> *>(&ch.sampler.output);
    // UTILS_PRINTLN(
    //"t: %.4f, prev_key: %llu, next_key: %llu", t, prev_key, next_key);

    return Lerp(output[prev_key], output[next_key], tn);
}

vec4
Interpolator::InterpolateVec4(const Channel &ch, float t, float max_time)
{
    const auto tn = Interpolate(ch, t, max_time);
    const auto output =
        *reinterpret_cast<const std::vector<vec4> *>(&ch.sampler.output);
    // UTILS_PRINTLN(
    //     "t: %.4f, prev_key: %llu, next_key: %llu", t, prev_key, next_key);

    return QuatSlerp(output[prev_key], output[next_key], tn);
}

std::vector<float>
Interpolator::InterpolateWeights(const Channel &ch,
                                 int len,
                                 float t,
                                 float max_time)
{
    const auto tn = Interpolate(ch, t, max_time);
    const auto output =
        *reinterpret_cast<const std::vector<float> *>(&ch.sampler.output);
    std::vector<float> weights;
    weights.reserve(len);
    for (int i = 0; i < len; ++i) {
        weights.push_back(Lerp(output[prev_key + i], output[next_key + i], tn));
    }
    return weights;
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
        if (t <= input[i]) {
            next_key = Clamp(i, 1LLU, input.size() - 1LLU);
            break;
        }
    }

    prev_key = Clamp(next_key - 1LLU, 0LLU, next_key);
    const auto key_delta = input[next_key] - input[prev_key];
    const auto tn = (t - input[prev_key]) / key_delta;
    UTILS_ASSERT(tn <= 1.0f, "Invalid normalization");
    return tn;
}

static RenderNode *
find_node_by_id(int id, std::vector<RenderNode> &render_nodes)
{
    for (auto &node : render_nodes) {
        if (node.id == id) {
            return &node;
        }
        if (!node.children.empty()) {
            return find_node_by_id(id, node.children);
        }
    }
    return nullptr;
}

void
Animation::Advance(float dt, std::vector<RenderNode> &render_nodes)
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

        const auto found_node = find_node_by_id(ch.target_node, render_nodes);
        UTILS_ASSERT(found_node, "For some reason cannod find target node");
        auto &transform = found_node->transform;

        switch (ch.target_path) {
            case Channel::Path::Translation:
            {
                const auto t =
                    interpolator.InterpolateVec3(ch, cur_time, max_time);
                transform.anim_translation = t;
                transform.updated = true;
            } break;
            case Channel::Path::Rotation:
            {
                const auto r =
                    interpolator.InterpolateVec4(ch, cur_time, max_time);
                transform.anim_rotation = r;
                transform.updated = true;
            } break;
            case Channel::Path::Scale:
            {
                const auto s =
                    interpolator.InterpolateVec3(ch, cur_time, max_time);
                transform.anim_scale = s;
                transform.updated = true;
            } break;
            case Channel::Path::Weights:
            {
                auto weights = interpolator.InterpolateWeights(
                    ch,
                    found_node->mesh.get_weights().size(),
                    cur_time,
                    max_time);
                found_node->mesh.set_weights(std::move(weights));
            } break;
            default:
                UTILS_ASSERT(false, "Invalid path");
        }
    }
}
}  // namespace NEngine::Renderer
