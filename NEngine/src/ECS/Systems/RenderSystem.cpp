#include "NEngine/ECS/Systems/RenderSystem.h"

#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/ECS/Components/RenderComponent.h"
#include "NEngine/Globals.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/NEMath.h"
#include "NEngine/Renderer/WireframePass.h"

namespace NEngine::ECS::Systems {

using namespace NEngine::Renderer;
using namespace NEngine::ECS::Components;
using namespace NEngine::Math;
using namespace NEngine::Helpers;

static const std::string COLLISION_MESH_PATH =
    UtilsFormatStr("%s/gLTF/cube.glb", NENGINE_RES_DIR);
static std::vector<std::unique_ptr<Mesh>> COLLISION_MESH;

RenderSystem::RenderSystem(NEngine::Helpers::DeviceResources &deviceResources,
                           ECS::DefaultEntityManager &entityManager)
    : mDeviceResources(&deviceResources),
      mBasePass(std::make_unique<BasePass>(deviceResources)),
      mEntityManager(&entityManager),
      mCamera(nullptr)
{
    mPasses.push_back(std::make_unique<WireframePass>(deviceResources));
    COLLISION_MESH = GEngine->LoadMesh(COLLISION_MESH_PATH);
    UTILS_ASSERT(COLLISION_MESH.size() > 0, "Failed to load collision mesh");
}

auto
RenderSystem::Update(float dt) -> void
{
    Clear();

    {
        static auto dirLightTime = 0.0f;
        dirLightTime += (dt / 1000);
        auto dirLight = mBasePass->GetBufferValue<DirectionalLight>(
            "dirLight", BufferType::PerScene);

        if (dirLight) {
            constexpr float radius = 100;
            dirLight->Direction = Vec4D(cos(dirLightTime) * radius,
                                        0,
                                        sin(dirLightTime) * radius,
                                        radius);
        }
    }

    for (auto entity : mEntities) {
        auto &pc = *mEntityManager->GetComponent<PositionComponent>(entity);
        const auto position = pc.Position;
        auto &rc = *mEntityManager->GetComponent<RenderComponent>(entity);
        for (auto &mesh : rc.Mesh) {
            // TODO: world matrix should be precomputed somewhere else.
            auto translate = MathMat4X4TranslateFromVec3D(&pc.Position);
            auto axis = vec3(0, 1, 0);
            auto rotate = XM::RotateAxis(ToRadians(pc.Yaw), axis);
            auto scales = vec3(pc.Scale, pc.Scale, pc.Scale);
            auto scale = MathMat4X4ScaleFromVec3D(&scales);
            mesh->GetTransform().SetWorld(scale * rotate * translate);
        }
        mBasePass->Draw(*mDeviceResources, rc.Mesh);

        if (mEntityManager->HasComponent<CollisionComponent>(entity)) {
            const auto &cc =
                *mEntityManager->GetComponent<CollisionComponent>(entity);
            const auto len = (cc.BoxMax - cc.BoxMin).Length();
            auto scale = vec3(len, len, len);
            for (auto &mesh : COLLISION_MESH) {
                auto &t = mesh->GetTransform();
                // t.SetScale(MathMat4X4ScaleFromVec3D(&scale));
                t.SetWorld(MathMat4X4ScaleFromVec3D(&scale) *
                           MathMat4X4TranslateFromVec3D(&position));
            }
            mPasses[0]->Draw(*mDeviceResources, COLLISION_MESH);
        }
    }
}

auto
RenderSystem::Clear() -> void
{
    ID3D11DeviceContext *ctx = mDeviceResources->GetDeviceContext();
    ID3D11RenderTargetView *rtv = mDeviceResources->GetRenderTargetView();
    ID3D11DepthStencilView *dsv = mDeviceResources->GetDepthStencilView();

    static const float CLEAR_COLOR[4] = {
        0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f};

    ctx->Flush();

    ctx->ClearRenderTargetView(rtv, CLEAR_COLOR);
    ctx->ClearDepthStencilView(
        dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    ctx->OMSetRenderTargets(1, &rtv, dsv);
    ctx->RSSetViewports(1, &mDeviceResources->GetViewport());
}

auto
RenderSystem::RegisterEntity(Entity entity) -> void
{
    if (mEntityManager->HasComponent<PositionComponent>(entity) &&
        mEntityManager->HasComponent<RenderComponent>(entity)) {
        if (mEntityManager->HasComponent<CameraComponent>(entity)) {
            auto &camComp =
                *mEntityManager->GetComponent<CameraComponent>(entity);
            mCamera = &camComp.Camera;
            mBasePass->SetCamera(*mCamera);
            for (auto &pass : mPasses) {
                pass->SetCamera(*mCamera);
            }
        }

        auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
        if (it != std::end(mEntities)) {
            UTILS_PRINTLN("Entity %l already registered with RenderSystem");
            return;
        }

        mEntities.push_back(entity);
    }
}

auto
RenderSystem::UnregisterEntity(Entity entity) -> void
{
    auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
    if (it != std::end(mEntities)) {
        mEntities.erase(it);
    }
}

auto
RenderSystem::ReloadShaders() -> void
{
    mBasePass->ReloadShaders();
}

}  // namespace NEngine::ECS::Systems