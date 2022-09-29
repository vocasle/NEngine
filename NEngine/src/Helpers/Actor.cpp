//
// Created by vocasle on 8/4/2022.
//

#include "NEngine/Helpers/Actor.h"

namespace NEngine {
namespace Helpers {
using namespace Math;

Actor::Actor()
    : m_isVisible(true),
      m_world(MathMat4X4Identity()),
      m_roughness(1) {
};

Actor::Actor(std::vector<Mesh> meshes)
    : m_meshes(std::move(meshes)),
      m_isVisible(true),
      m_world(MathMat4X4Identity()),
      m_roughness(1) {
}

const std::vector<Mesh> &
Actor::GetMeshes() const {
    return m_meshes;
}

void
Actor::SetVisible(bool isVisible) {
    m_isVisible = isVisible;
}

void
Actor::SetWorld(const Mat4X4 &world) {
    m_world = world;
}

bool
Actor::IsVisible() const {
    return m_isVisible;
}

Mat4X4
Actor::GetWorld() const {
    return m_world;
}

float
Actor::GetRoughness() const {
    return m_roughness;
}

void
Actor::SetRoughness(float roughness) {
    m_roughness = roughness;
}

}
}
