//
// Created by vocasle on 8/4/2022.
//

#pragma once

#include <vector>

#include "ModelLoader.h"
#include "NEngine/Math/Math.h"

namespace NEngine {
namespace Helpers {
class Actor {
public:
    Actor();
    explicit Actor(std::vector<Mesh> meshes);

    [[nodiscard]] bool IsVisible() const;
    [[nodiscard]] const std::vector<Mesh> &GetMeshes() const;
    [[nodiscard]] Math::Mat4X4 GetWorld() const;
    [[nodiscard]] float GetRoughness() const;

    void SetVisible(bool isVisible);
    void SetWorld(const Math::Mat4X4 &world);
    void SetRoughness(float roughness);

private:
    std::vector<Mesh> m_meshes;
    bool m_isVisible;
    Math::Mat4X4 m_world;
    float m_roughness;
};

}
}