#include "Sandbox/Game.h"
#include "Camera.h"
#include "DynamicConstBuffer.h"
#include "MeshGenerator.h"
#include "Mouse.h"
#include "Texture.h"
#include "Utils.h"

#include <d3dcompiler.h>

#if WITH_IMGUI
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>
#endif

using namespace Microsoft::WRL;
using namespace NEngine::Math;

namespace
{
Vec3D cubePositions[] = {
	{ 2.794f, 0.441f, 0.294f },
	{ -0.735f, 1.324f, 3.088f },
	{ -1.0f, -1.324f, -1.765f },
};

Vec3D cubeRotations[] = {
	{ MathToRadians(-42.0f), MathToRadians(53.0f), 0.0f },
	{ MathToRadians(45.0f), 0.0f, MathToRadians(45.0f) },
	{ MathToRadians(11.0f), MathToRadians(-37.0f), MathToRadians(15.0f) },
};

float cubeScales[] = {
	0.854f,
	0.8f,
	1.357f,
};

Vec4D g_PointLightColors[] = { { 1, 1, 0, 1 },
			       { 1, 0, 1, 1 },
			       { 1, 0, 1, 1 },
			       { 1, 1, 0, 1 } };

D3D11_RASTERIZER_DESC g_rasterizerDesc = { CD3D11_RASTERIZER_DESC{
	CD3D11_DEFAULT{} } };

std::unique_ptr<Texture> g_OffscreenRTV;
std::unique_ptr<Texture> g_BrightessRTV;
std::unique_ptr<Texture> g_BlurRTV;
std::unique_ptr<Texture> g_BlurRTV2;
std::unique_ptr<DynamicConstBuffer> g_FogCBuf;
std::unique_ptr<DynamicConstBuffer> g_LightCBuf;
std::unique_ptr<DynamicConstBuffer> g_BlurCBuf;

struct BloomSettings {
	bool isEnabled;
	bool isOnlyBrightness;
	bool isOnlyColor;
	bool isOnlyBlur;
	float threshold;
};

BloomSettings g_BloomSettings = { false, false, false, false, 1.0f };
};

static void GameUpdateConstantBuffer(ID3D11DeviceContext *context,
				     size_t bufferSize, void *data,
				     ID3D11Buffer *dest)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};

	if (FAILED(context->Map((ID3D11Resource *)dest, 0,
				D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
		UtilsFatalError("ERROR: Failed to map constant buffer\n");
	}
	memcpy(mapped.pData, data, bufferSize);
	context->Unmap((ID3D11Resource *)dest, 0);
}

static void GameCreateConstantBuffer(ID3D11Device *device, size_t byteWidth,
				     ID3D11Buffer **pDest)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&bufferDesc, NULL, pDest))) {
		UtilsFatalError(
			"ERROR: Failed to create per frame constants cbuffer\n");
	}
}

Actor *Game::FindActorByName(const std::string &name)
{
	auto it = std::find_if(std::begin(m_Actors), std::end(m_Actors),
			       [&name](const Actor &actor) {
				       return actor.GetName() == name;
			       });
	return it == std::end(m_Actors) ? nullptr : &*it;
}

// TODO: Update this to get list of actors to draw
void Game::DrawScene()
{
	m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
				      m_PerFrameCB.Get(), 1);
	m_Renderer.BindConstantBuffer(BindTargets::PixelShader,
				      m_PerFrameCB.Get(), 1);

	m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
				      m_PerSceneCB.Get(), 2);
	m_Renderer.BindConstantBuffer(BindTargets::PixelShader,
				      m_PerSceneCB.Get(), 2);

	for (size_t i = 0; i < m_Actors.size(); ++i) {
		Actor &actor = m_Actors[i];
		if (actor.GetName() == "Cube") {
			for (int i = 0; i < 3; ++i) {
				actor.Scale(cubeScales[i]);
				actor.Translate(cubePositions[i]);
				const float pitch = cubeRotations[i].X;
				const float yaw = cubeRotations[i].Y;
				const float roll = cubeRotations[i].Z;
				actor.Rotate(pitch, yaw, roll);
				DrawActor(actor);
			}
		} else {
			DrawActor(actor);
		}
	}
}

void Game::DrawSky()
{
	m_DR->PIXBeginEvent(L"Draw sky");
	m_Renderer.SetRasterizerState(m_CubeMap.GetRasterizerState());
	m_Renderer.SetDepthStencilState(m_CubeMap.GetDepthStencilState());
	m_Renderer.BindVertexShader(m_shaderManager.GetVertexShader("SkyVS"));
	m_Renderer.SetInputLayout(m_shaderManager.GetInputLayout());
	m_Renderer.BindPixelShader(m_shaderManager.GetPixelShader("SkyPS"));
	m_Renderer.BindShaderResource(BindTargets::PixelShader,
				      m_CubeMap.GetCubeMap(), 0);
	m_Renderer.SetSamplerState(m_CubeMap.GetCubeMapSampler(), 0);
	m_Renderer.SetVertexBuffer(m_CubeMap.GetVertexBuffer(),
				   m_shaderManager.GetStrides(), 0);
	m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
				      m_PerObjectCB.Get(), 0);
	m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
				      m_PerFrameCB.Get(), 1);
	m_Renderer.SetIndexBuffer(m_CubeMap.GetIndexBuffer(), 0);
	m_Renderer.DrawIndexed(m_CubeMap.GetNumIndices(), 0, 0);
	m_DR->PIXEndEvent();
}

void Game::CreateRasterizerState()
{
	if (FAILED(m_DR->GetDevice()->CreateRasterizerState(
		    &g_rasterizerDesc,
		    m_rasterizerState.ReleaseAndGetAddressOf()))) {
		OutputDebugStringA(
			"ERROR: Failed to create rasterizer state\n");
		ExitProcess(EXIT_FAILURE);
	}
}

#if WITH_IMGUI
void Game::UpdateImgui()
{
	// Any application code here
	{
		static_assert(sizeof(float) * 3 == sizeof(Vec3D),
			      "ERROR: Cannot cast Vec3D to float[3]");
		static float zNear = 0.1f;
		static float zFar = 1000.0f;
		ImGui::PushItemWidth(150.0f);
		ImGui::InputFloat("Z near", &zNear);
		ImGui::SameLine();
		ImGui::InputFloat("Z far", &zFar);
		ImGui::PopItemWidth();
		ImGui::InputFloat3("Dir light pos",
				   reinterpret_cast<float *>(
					   &m_PerSceneData.dirLight.Position));
		m_Camera.SetZFar(zFar);
		m_Camera.SetZNear(zNear);
	}

	if (ImGui::CollapsingHeader("Bloom settings")) {
		ImGui::Checkbox("Enabled", &g_BloomSettings.isEnabled);
		ImGui::Checkbox("Only brightness",
				&g_BloomSettings.isOnlyBrightness);
		ImGui::Checkbox("Only blur", &g_BloomSettings.isOnlyBlur);
		ImGui::Checkbox("Only color", &g_BloomSettings.isOnlyColor);
		ImGui::SliderFloat("Threshold", &g_BloomSettings.threshold,
				   0.0f, 5.0f);
	}

	if (ImGui::CollapsingHeader("Rasterizer settings")) {
		ImGui::PushItemWidth(150.0f);
		ImGui::SliderInt(
			"FillMode",
			reinterpret_cast<int *>(&g_rasterizerDesc.FillMode), 2,
			3);
		ImGui::Checkbox(
			"FrontCounterClockwise",
			reinterpret_cast<bool *>(
				&g_rasterizerDesc.FrontCounterClockwise));
		ImGui::SliderInt("DepthBias", &g_rasterizerDesc.DepthBias, 0,
				 10000);
		ImGui::SliderInt(
			"CullMode",
			reinterpret_cast<int *>(&g_rasterizerDesc.CullMode), 1,
			3);
		ImGui::SliderFloat("DepthBiasClamp",
				   &g_rasterizerDesc.DepthBiasClamp, -1000.0f,
				   1000.0f);
		ImGui::SliderFloat("SlopeScaledDepthBias",
				   &g_rasterizerDesc.SlopeScaledDepthBias,
				   -1000.0f, 1000.0f);
		ImGui::Checkbox("DepthClipEnable",
				reinterpret_cast<bool *>(
					&g_rasterizerDesc.DepthClipEnable));
		ImGui::Checkbox("ScissorEnable",
				reinterpret_cast<bool *>(
					&g_rasterizerDesc.ScissorEnable));
		ImGui::Checkbox("MultisampleEnable",
				reinterpret_cast<bool *>(
					&g_rasterizerDesc.MultisampleEnable));
		ImGui::Checkbox(
			"AntialiasedLineEnable",
			reinterpret_cast<bool *>(
				&g_rasterizerDesc.AntialiasedLineEnable));
		ImGui::PopItemWidth();
		CreateRasterizerState();
	}

	if (ImGui::Button("Compile")) {
		m_shaderManager.Recompile(m_DR->GetDevice());
	}

	for (int i = 0; i < 3; ++i) {
		if (ImGui::CollapsingHeader(
			    UtilsFormatStr("Cube %d", i).c_str())) {
			ImGui::PushItemWidth(150.0f);
			ImGui::SliderFloat(
				UtilsFormatStr("Cube %d scale", i).c_str(),
				&cubeScales[i], 0.1f, 2.0f);
			ImGui::SliderFloat(
				UtilsFormatStr("Cube %d Pos.X", i).c_str(),
				&cubePositions[i].X, -10.0f, 10.0f);
			ImGui::SameLine();
			ImGui::SliderFloat(
				UtilsFormatStr("Cube %d Pos.Y", i).c_str(),
				&cubePositions[i].Y, -10.0f, 10.0f);
			ImGui::SameLine();
			ImGui::SliderFloat(
				UtilsFormatStr("Cube %d Pos.Z", i).c_str(),
				&cubePositions[i].Z, -10.0f, 10.0f);
			ImGui::SliderAngle(
				UtilsFormatStr("Cube %d Pitch", i).c_str(),
				&cubeRotations[i].X);
			ImGui::SameLine();
			ImGui::SliderAngle(
				UtilsFormatStr("Cube %d Yaw", i).c_str(),
				&cubeRotations[i].Y);
			ImGui::SameLine();
			ImGui::SliderAngle(
				UtilsFormatStr("Cube %d Roll", i).c_str(),
				&cubeRotations[i].Z);
			ImGui::PopItemWidth();
		}
	}
}
#endif

void Game::CreateDefaultSampler()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(m_DR->GetDevice()->CreateSamplerState(
		    &samplerDesc, m_DefaultSampler.ReleaseAndGetAddressOf()))) {
		UtilsDebugPrint(
			"ERROR: Failed to create default sampler state\n");
		ExitProcess(EXIT_FAILURE);
	}
}

void Game::InitPerSceneConstants()
{
	struct PointLight pl = {};
	const Vec3D positions[] = {
		{ -4.0f, 0.4f, -4.0f },
		{ -4.0f, 1.5f, 4.0f },
		{ 4.0f, 1.5f, 4.0f },
		{ 4.0f, 0.5f, -4.0f },
	};

	for (uint32_t i = 0; i < 4; ++i) {
		pl.Position = positions[i];
		pl.Ambient = ColorFromRGBA(0.03f, 0.03f, 0.03f, 1.0f);
		pl.Diffuse = *reinterpret_cast<Color *>(&g_PointLightColors[i]);
		pl.Specular = ColorFromRGBA(0.2f, 0.2f, 0.2f, 1.0f);
		pl.Att = MathVec3DFromXYZ(1.0f, 0.09f, 0.032f);
		pl.Range = 5.0f;
		m_PerSceneData.pointLights[i] = pl;
	}

	DirectionalLight dirLight = {};
	dirLight.Ambient = ColorFromRGBA(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight.Diffuse = ColorFromRGBA(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Specular = ColorFromRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	dirLight.Position = MathVec3DFromXYZ(5.0f, 5.0f, 5.0f);
	m_PerSceneData.dirLight = dirLight;

	SpotLight spotLight = {};
	spotLight.Position = m_Camera.GetPos();
	spotLight.Direction = m_Camera.GetAt();
	spotLight.Ambient = ColorFromRGBA(0.1f, 0.1f, 0.1f, 1.0f);
	spotLight.Diffuse = ColorFromRGBA(1.0f, 0.0f, 0.0f, 1.0f);
	spotLight.Specular = ColorFromRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight.Att = MathVec3DFromXYZ(1.0f, 0.09f, 0.032f);
	spotLight.Range = 5.0f;
	spotLight.Spot = 32.0f;
	m_PerSceneData.spotLights[0] = spotLight;
}

Game::Game()
	: m_Camera{ { 0.0f, 0.0f, -5.0f } }
{
	m_DR = std::make_unique<DeviceResources>();
}

Game::~Game()
{
#if WITH_IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void Game::Clear()
{
	ID3D11DeviceContext *ctx = m_DR->GetDeviceContext();
	ID3D11RenderTargetView *rtv = m_DR->GetRenderTargetView();
	ID3D11DepthStencilView *dsv = m_DR->GetDepthStencilView();

	static const float CLEAR_COLOR[4] = { 0.392156899f, 0.584313750f,
					      0.929411829f, 1.000000000f };

	ctx->Flush();

	ctx->ClearRenderTargetView(rtv, CLEAR_COLOR);
	ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
				   1.0f, 0);
	ctx->OMSetRenderTargets(1, &rtv, dsv);
	ctx->RSSetViewports(1, &m_DR->GetViewport());
}

void Game::Update()
{
	m_Camera.ProcessKeyboard(m_Timer.DeltaMillis);
	m_Camera.ProcessMouse(m_Timer.DeltaMillis);

	m_PerFrameData.view = m_Camera.GetViewMat();
	m_PerFrameData.proj = m_Camera.GetProjMat();
	m_PerFrameData.cameraPosW = m_Camera.GetPos();

	GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
				 sizeof(PerFrameConstants), &m_PerFrameData,
				 m_PerFrameCB.Get());
	GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
				 sizeof(PerSceneConstants), &m_PerSceneData,
				 m_PerSceneCB.Get());

	static float elapsedTime = 0.0f;
	const float deltaSeconds =
		static_cast<float>(m_Timer.DeltaMillis / 1000.0);
	elapsedTime += deltaSeconds;

	if (elapsedTime >= 1.0f) {
		SetWindowText(m_DR->GetWindow(),
			      UtilsFormatStr("bloom -- FPS: %d, frame: %f s",
					     static_cast<int>(elapsedTime /
							      deltaSeconds),
					     deltaSeconds)
				      .c_str());
		elapsedTime = 0.0f;
	}

	g_FogCBuf->SetValue("viewInverse",
			    MathMat4X4Inverse(m_Camera.GetViewMat()));
	g_FogCBuf->SetValue("projInverse",
			    MathMat4X4Inverse(m_Camera.GetProjMat()));
	Vec4D camPos = { m_Camera.GetPos(), 1 };
	const Mat4X4 vp = m_Camera.GetViewMat() * m_Camera.GetProjMat();
	camPos = MathMat4X4MultVec4DByMat4X4(&camPos, &vp);
	camPos = camPos / camPos.W;
	g_FogCBuf->SetValue("cameraPos", Vec3D(camPos.X, camPos.Y, camPos.Z));
	g_FogCBuf->SetValue("fogColor", *reinterpret_cast<Vec4D *>(
						&m_PerSceneData.fogColor));
	g_FogCBuf->SetValue("fogEnd", m_PerSceneData.fogEnd);
	g_FogCBuf->SetValue("fogStart", m_PerSceneData.fogStart);
	g_FogCBuf->UpdateConstantBuffer(m_DR->GetDeviceContext());

#if WITH_IMGUI
	static float totalTime = 0.0f;
	totalTime += deltaSeconds;
#endif
}

void Game::Render()
{
#if WITH_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

#if WITH_IMGUI
	UpdateImgui();
#endif

	static constexpr float BLACK_COLOR[] = { 0, 0, 0, 1 };
	m_Renderer.SetRenderTargets(m_DR->GetRenderTargetView(),
				    m_DR->GetDepthStencilView());
	m_Renderer.SetViewport(m_DR->GetViewport());
	m_Renderer.Clear(nullptr);
	m_Renderer.SetBlendState(nullptr);
	m_Renderer.SetDepthStencilState(nullptr);
	m_Renderer.SetPrimitiveTopology(R_DEFAULT_PRIMTIVE_TOPOLOGY);
	m_Renderer.SetRasterizerState(m_rasterizerState.Get());
	m_Renderer.SetSamplerState(m_DefaultSampler.Get(), 0);

	m_DR->PIXBeginEvent(L"Color pass");
	// reset view proj matrix back to camera
	{
		// draw to offscreen texture
		m_Renderer.SetRenderTargets(g_OffscreenRTV->GetRTV(),
					    g_OffscreenRTV->GetDSV());
		m_Renderer.Clear(nullptr);
		m_PerFrameData.view = m_Camera.GetViewMat();
		m_PerFrameData.proj = m_Camera.GetProjMat();
		m_PerFrameData.cameraPosW = m_Camera.GetPos();
		GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
					 sizeof(PerFrameConstants),
					 &m_PerFrameData, m_PerFrameCB.Get());
		m_Renderer.BindVertexShader(
			m_shaderManager.GetVertexShader("ColorVS"));
		m_Renderer.BindPixelShader(
			m_shaderManager.GetPixelShader("PhongPS"));
		m_Renderer.SetSamplerState(m_DefaultSampler.Get(), 0);
		m_Renderer.SetSamplerState(m_ShadowMap.GetShadowSampler(), 1);
		m_Renderer.SetInputLayout(m_shaderManager.GetInputLayout());
		m_Renderer.BindShaderResource(BindTargets::PixelShader,
					      m_ShadowMap.GetDepthMapSRV(), 4);

		DrawScene();
	}
	m_DR->PIXEndEvent();
	m_DR->PIXBeginEvent(L"Light");
	// Light properties
	for (uint32_t i = 0; i < _countof(m_PerSceneData.pointLights); ++i) {
		const Vec3D scale = { 0.5f, 0.5f, 0.5f };
		Mat4X4 world = MathMat4X4ScaleFromVec3D(&scale);
		world = world *
			MathMat4X4TranslateFromVec3D(
				&m_PerSceneData.pointLights[i].Position);
		m_PerObjectData.world = world;
		GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
					 sizeof(PerObjectConstants),
					 &m_PerObjectData, m_PerObjectCB.Get());

		g_LightCBuf->SetValue("color",
				      m_PerSceneData.pointLights[i].Diffuse);
		g_LightCBuf->UpdateConstantBuffer(m_DR->GetDeviceContext());
		m_Renderer.BindConstantBuffer(BindTargets::PixelShader,
					      g_LightCBuf->Get(), 0);
		m_Renderer.BindPixelShader(
			m_shaderManager.GetPixelShader("LightPS"));
		m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
					      m_PerObjectCB.Get(), 0);
		m_Renderer.SetInputLayout(m_shaderManager.GetInputLayout());
		const auto lightSource = FindActorByName("Cube");
		m_Renderer.SetIndexBuffer(lightSource->GetIndexBuffer(), 0);
		m_Renderer.SetVertexBuffer(lightSource->GetVertexBuffer(),
					   m_shaderManager.GetStrides(), 0);
		m_Renderer.DrawIndexed(lightSource->GetNumIndices(), 0, 0);
	}

	{
		const Vec3D scale = { 0.5f, 0.5f, 0.5f };
		Mat4X4 world = MathMat4X4ScaleFromVec3D(&scale);
		world = world * MathMat4X4TranslateFromVec3D(
					&m_PerSceneData.dirLight.Position);
		m_PerObjectData.world = world;
		GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
					 sizeof(PerObjectConstants),
					 &m_PerObjectData, m_PerObjectCB.Get());

		g_LightCBuf->SetValue("color", Vec4D(1.0f, 1.0f, 1.0f, 1.0f));
		g_LightCBuf->UpdateConstantBuffer(m_DR->GetDeviceContext());
		m_Renderer.BindConstantBuffer(BindTargets::PixelShader,
					      g_LightCBuf->Get(), 0);
		m_Renderer.BindPixelShader(
			m_shaderManager.GetPixelShader("LightPS"));
		m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
					      m_PerObjectCB.Get(), 0);
		const auto lightSource = FindActorByName("Cube");
		m_Renderer.SetIndexBuffer(lightSource->GetIndexBuffer(), 0);
		m_Renderer.SetVertexBuffer(lightSource->GetVertexBuffer(),
					   m_shaderManager.GetStrides(), 0);
		m_Renderer.DrawIndexed(lightSource->GetNumIndices(), 0, 0);
	}
	m_DR->PIXEndEvent();

	// draw sky
	// DrawSky();

	// Brightness pass
	m_DR->PIXBeginEvent(L"Brightness");
	{
		m_Renderer.SetRenderTargets(g_BrightessRTV->GetRTV(), nullptr);
		m_Renderer.Clear(BLACK_COLOR);
		m_Renderer.BindVertexShader(
			m_shaderManager.GetVertexShader("FogVS"));
		m_Renderer.BindPixelShader(
			m_shaderManager.GetPixelShader("BrightPS"));
		m_Renderer.SetInputLayout(m_shaderManager.GetInputLayout());
		m_Renderer.BindShaderResource(BindTargets::PixelShader,
					      g_OffscreenRTV->GetSRV(), 0);
		m_Renderer.SetSamplerState(m_DefaultSampler.Get(), 0);

		const auto fogPlane = FindActorByName("FogPlane");
		m_Renderer.SetVertexBuffer(fogPlane->GetVertexBuffer(),
					   sizeof(Vertex), 0);
		m_Renderer.SetIndexBuffer(fogPlane->GetIndexBuffer(), 0);
		m_Renderer.DrawIndexed(fogPlane->GetNumIndices(), 0, 0);
	}
	m_DR->PIXEndEvent();

	// Blur pass
	m_DR->PIXBeginEvent(L"Blur");
	{
		m_Renderer.ClearRenderTargetView(g_BlurRTV->GetRTV(),
						 BLACK_COLOR);
		m_Renderer.ClearRenderTargetView(g_BlurRTV2->GetRTV(),
						 BLACK_COLOR);
		m_Renderer.BindPixelShader(
			m_shaderManager.GetPixelShader("BlurPS"));
		const auto fogPlane = FindActorByName("FogPlane");
		m_Renderer.SetVertexBuffer(fogPlane->GetVertexBuffer(),
					   sizeof(Vertex), 0);
		m_Renderer.SetIndexBuffer(fogPlane->GetIndexBuffer(), 0);
		g_BlurCBuf->SetValue(
			"width",
			static_cast<float>(m_DR->GetOutputSize().right));
		g_BlurCBuf->SetValue(
			"height",
			static_cast<float>(m_DR->GetOutputSize().bottom));

		bool isHorizontal = true;
		bool isFirstRun = true;
		for (int i = 0; i < 10; ++i) {
			g_BlurCBuf->SetValue("isHorizontal",
					     isHorizontal ? 1 : 0);
			g_BlurCBuf->UpdateConstantBuffer(
				m_DR->GetDeviceContext());
			m_Renderer.BindConstantBuffer(BindTargets::PixelShader,
						      g_BlurCBuf->Get(), 0);
			if (isHorizontal) {
				m_Renderer.SetRenderTargets(g_BlurRTV->GetRTV(),
							    nullptr);
				if (isFirstRun) {
					isFirstRun = false;
					m_Renderer.BindShaderResource(
						BindTargets::PixelShader,
						g_BrightessRTV->GetSRV(), 0);
				} else {
					m_Renderer.BindShaderResource(
						BindTargets::PixelShader,
						g_BlurRTV2->GetSRV(), 0);
				}
			} else {
				m_Renderer.SetRenderTargets(
					g_BlurRTV2->GetRTV(), nullptr);
				m_Renderer.BindShaderResource(
					BindTargets::PixelShader,
					g_BlurRTV->GetSRV(), 0);
			}
			m_Renderer.DrawIndexed(fogPlane->GetNumIndices(), 0, 0);
			m_Renderer.BindShaderResource(BindTargets::PixelShader,
						      nullptr, 0);
			m_Renderer.SetRenderTargets(nullptr, nullptr);
			isHorizontal = !isHorizontal;
		}
	}
	m_DR->PIXEndEvent();

	m_DR->PIXBeginEvent(L"Bloom");
	{
		g_FogCBuf->SetValue("world", MathMat4X4RotateX(90.0f));
		g_FogCBuf->UpdateConstantBuffer(m_DR->GetDeviceContext());
		m_Renderer.SetRenderTargets(m_DR->GetRenderTargetView(),
					    m_DR->GetDepthStencilView());
		m_Renderer.BindVertexShader(
			m_shaderManager.GetVertexShader("FogVS"));
		m_Renderer.BindPixelShader(
			m_shaderManager.GetPixelShader("BloomPS"));
		m_Renderer.SetInputLayout(m_shaderManager.GetInputLayout());
		m_Renderer.BindShaderResource(BindTargets::PixelShader,
					      g_OffscreenRTV->GetSRV(), 0);
		m_Renderer.BindShaderResource(BindTargets::PixelShader,
					      g_BlurRTV->GetSRV(), 1);
		m_Renderer.BindShaderResource(BindTargets::PixelShader,
					      g_BlurRTV2->GetSRV(), 2);
		m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
					      g_FogCBuf->Get(), 0);

		const auto fogPlane = FindActorByName("FogPlane");
		m_Renderer.SetVertexBuffer(fogPlane->GetVertexBuffer(),
					   sizeof(Vertex), 0);
		m_Renderer.SetIndexBuffer(fogPlane->GetIndexBuffer(), 0);

		m_Renderer.DrawIndexed(fogPlane->GetNumIndices(), 0, 0);

		// unbind resources
		m_Renderer.BindShaderResource(BindTargets::PixelShader, nullptr,
					      0);
		m_Renderer.BindShaderResource(BindTargets::PixelShader, nullptr,
					      1);
		m_Renderer.BindShaderResource(BindTargets::PixelShader, nullptr,
					      2);
	}
	m_DR->PIXEndEvent();

#if WITH_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	m_Renderer.Present();
}

void Game::Tick()
{
	TimerTick(&m_Timer);
	Update();
	Render();
}

void Game::CreateActors()
{
	const Material material = { { 0.24725f, 0.1995f, 0.0745f, 1.0f },
				    { 0.75164f, 0.60648f, 0.22648f, 1.0f },
				    { 0.628281f, 0.555802f, 0.366065f, 32.0f },
				    { 0.0f, 0.0f, 0.0f, 0.0f } };

	// load cube
	{
		Actor actor = Actor();
		actor.LoadModel(
			UtilsFormatStr("%s/meshes/cube.obj", ASSETS_ROOT)
				.c_str());
		actor.CreateIndexBuffer(m_DR->GetDevice());
		actor.CreateVertexBuffer(m_DR->GetDevice());
		actor.SetName("Cube");
		actor.SetIsVisible(true);
		actor.SetMaterial(&material);
		m_Actors.emplace_back(actor);
	}

	// load plane
	{
		const Vec3D origin = { 0.0f, 0.0f, 0.0f };
		std::unique_ptr<Mesh> mesh =
			MGGeneratePlane(&origin, 500.0f, 500.0f);
		Actor plane = Actor(mesh.get());
		plane.CreateIndexBuffer(m_DR->GetDevice());
		plane.CreateVertexBuffer(m_DR->GetDevice());
		const Vec3D offset = { 0.0f, -1.0f, 0.0f };
		plane.Translate(offset);

		plane.LoadTexture(
			UtilsFormatStr("%s/textures/bricks_diffuse.jpg",
				       ASSETS_ROOT)
				.c_str(),
			TextureType::Diffuse, m_DR->GetDevice(),
			m_DR->GetDeviceContext());
		plane.LoadTexture(
			UtilsFormatStr("%s/textures/bricks_normal.png",
				       ASSETS_ROOT)
				.c_str(),
			TextureType::Normal, m_DR->GetDevice(),
			m_DR->GetDeviceContext());
		plane.LoadTexture(UtilsFormatStr("%s/textures/bricks_gloss.jpg",
						 ASSETS_ROOT)
					  .c_str(),
				  TextureType::Gloss, m_DR->GetDevice(),
				  m_DR->GetDeviceContext());
		plane.LoadTexture(
			UtilsFormatStr("%s/textures/bricks_reflection.jpg",
				       ASSETS_ROOT)
				.c_str(),
			TextureType::Specular, m_DR->GetDevice(),
			m_DR->GetDeviceContext());
		plane.SetMaterial(&material);
		plane.SetIsVisible(true);
		plane.SetName("Plane");
		m_Actors.emplace_back(plane);
	}

	{
		Mesh mesh = {};
		mesh.Positions.push_back({ -1.0f, 1.0f, 0.0f });
		mesh.Positions.push_back({ 1.0f, 1.0f, 0.0f });
		mesh.Positions.push_back({ -1.0f, -1.0f, 0.0f });
		mesh.Positions.push_back({ 1.0f, -1.0f, 0.0f });
		mesh.TexCoords.push_back({ 0.0f, 0.0f });
		mesh.TexCoords.push_back({ 1.0f, 0.0f });
		mesh.TexCoords.push_back({ 0.0f, 1.0f });
		mesh.TexCoords.push_back({ 1.0f, 1.0f });
		mesh.Faces.push_back({ 0, 0, 0 });
		mesh.Faces.push_back({ 1, 1, 0 });
		mesh.Faces.push_back({ 2, 2, 0 });
		mesh.Faces.push_back({ 2, 2, 0 });
		mesh.Faces.push_back({ 1, 1, 0 });
		mesh.Faces.push_back({ 3, 3, 0 });
		mesh.Normals.push_back({});
		Actor plane = Actor(&mesh);
		plane.CreateIndexBuffer(m_DR->GetDevice());
		plane.CreateVertexBuffer(m_DR->GetDevice());
		plane.SetIsVisible(false);
		plane.SetName("FogPlane");
		m_Actors.emplace_back(plane);
	}

	Actor *p = FindActorByName("Plane");
	Actor *c = FindActorByName("Cube");
	c->SetTextures(p->GetShaderResources());
}

void Game::Initialize(HWND hWnd, uint32_t width, uint32_t height)
{
	using namespace Microsoft::WRL;
#ifdef MATH_TEST
	MathTest();
#endif
	m_DR->SetWindow(hWnd, width, height);
	m_DR->CreateDeviceResources();
	m_DR->CreateWindowSizeDependentResources();
	TimerInitialize(&m_Timer);
	Mouse::Get().SetWindowDimensions(m_DR->GetBackBufferWidth(),
					 m_DR->GetBackBufferHeight());
	ID3D11Device *device = m_DR->GetDevice();

	m_ShadowMap.InitResources(device, 1024, 1024);
	m_Camera.SetViewDimensions(m_DR->GetBackBufferWidth(),
				   m_DR->GetBackBufferHeight());
	m_CubeMap.LoadCubeMap(
		device,
		{
			UtilsFormatStr("%s/textures/right.jpg", ASSETS_ROOT)
				.c_str(),
			UtilsFormatStr("%s/textures/left.jpg", ASSETS_ROOT)
				.c_str(),
			UtilsFormatStr("%s/textures/top.jpg", ASSETS_ROOT)
				.c_str(),
			UtilsFormatStr("%s/textures/bottom.jpg", ASSETS_ROOT)
				.c_str(),
			UtilsFormatStr("%s/textures/front.jpg", ASSETS_ROOT)
				.c_str(),
			UtilsFormatStr("%s/textures/back.jpg", ASSETS_ROOT)
				.c_str(),
		});

	m_dynamicCubeMap.Init(device);
	m_dynamicCubeMap.BuildCubeFaceCamera({ 0.0f, 0.0f, 0.0f });
	// init actors
	CreateActors();
	m_CubeMap.CreateCube(*FindActorByName("Cube"), device);
	InitPerSceneConstants();
	m_shaderManager.Initialize(device, SHADERS_ROOT,
				   UtilsFormatStr("%s/shader", SRC_ROOT));

	GameCreateConstantBuffer(device, sizeof(PerSceneConstants),
				 &m_PerSceneCB);
	GameCreateConstantBuffer(device, sizeof(PerObjectConstants),
				 &m_PerObjectCB);
	GameCreateConstantBuffer(device, sizeof(PerFrameConstants),
				 &m_PerFrameCB);
	GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
				 sizeof(PerSceneConstants), &m_PerSceneData,
				 m_PerSceneCB.Get());

	CreateDefaultSampler();
	g_rasterizerDesc.DepthBias = 500;
	CreateRasterizerState();

	m_Renderer.SetDeviceResources(m_DR.get());

	g_OffscreenRTV = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
						   m_DR->GetOutputSize().right,
						   m_DR->GetOutputSize().bottom,
						   m_DR->GetDevice());

	g_BrightessRTV = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
						   m_DR->GetOutputSize().right,
						   m_DR->GetOutputSize().bottom,
						   m_DR->GetDevice());

	g_BlurRTV = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
					      m_DR->GetOutputSize().right,
					      m_DR->GetOutputSize().bottom,
					      m_DR->GetDevice());

	g_BlurRTV2 = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
					       m_DR->GetOutputSize().right,
					       m_DR->GetOutputSize().bottom,
					       m_DR->GetDevice());

	{
		DynamicConstBufferDesc desc = {};
		desc.AddNode({ "fogEnd", NodeType::Float });
		desc.AddNode({ "fogStart", NodeType::Float });
		desc.AddNode({ "width", NodeType::Float });
		desc.AddNode({ "height", NodeType::Float });
		desc.AddNode({ "fogColor", NodeType::Float4 });
		desc.AddNode({ "world", NodeType::Float4X4 });
		desc.AddNode({ "viewInverse", NodeType::Float4X4 });
		desc.AddNode({ "projInverse", NodeType::Float4X4 });
		desc.AddNode({ "cameraPos", NodeType::Float3 });
		desc.AddNode({ "_pad1", NodeType::Float });
		g_FogCBuf = std::make_unique<DynamicConstBuffer>(desc);
		g_FogCBuf->SetValue("width", m_DR->GetOutputSize().right);
		g_FogCBuf->SetValue("height", m_DR->GetOutputSize().bottom);
		g_FogCBuf->CreateConstantBuffer(m_DR->GetDevice());
	}

	{
		DynamicConstBufferDesc desc = {};
		desc.AddNode({ "color", NodeType::Float4 });
		// desc.AddNode({"world", NodeType::Float4X4});
		// desc.AddNode({"view", NodeType::Float4X4});
		// desc.AddNode({"projection", NodeType::Float4X4});
		g_LightCBuf = std::make_unique<DynamicConstBuffer>(desc);
		g_LightCBuf->CreateConstantBuffer(m_DR->GetDevice());
	}

	{
		DynamicConstBufferDesc desc = {};
		desc.AddNode({ "width", NodeType::Float });
		desc.AddNode({ "height", NodeType::Float });
		desc.AddNode({ "isHorizontal", NodeType::Bool });
		desc.AddNode({ "pad", NodeType::Float });
		g_BlurCBuf = std::make_unique<DynamicConstBuffer>(desc);
		g_BlurCBuf->CreateConstantBuffer(m_DR->GetDevice());
	}

	m_PerSceneData.fogColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	m_PerSceneData.fogStart = 0;
	m_PerSceneData.fogEnd = 1;

#if WITH_IMGUI
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	const std::string droidSansTtf = UtilsFormatStr(
		"%s/../imgui-1.87/misc/fonts/DroidSans.ttf", SRC_ROOT);
	io.Fonts->AddFontFromFileTTF(droidSansTtf.c_str(), 16.0f);

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_DR->GetDevice(), m_DR->GetDeviceContext());
#endif
}

void Game::GetDefaultSize(uint32_t *width, uint32_t *height)
{
	*width = DEFAULT_WIN_WIDTH;
	*height = DEFAULT_WIN_HEIGHT;
}

void Game::BuildShadowTransform(Mat4X4 &view, Mat4X4 &proj)
{
	// Only the first "main" light casts a shadow.
	const Vec3D lightPos = m_PerSceneData.dirLight.Position;
	const Vec3D targetPos = { 0.0f, 0.0f, 0.0f };
	const Vec3D worldUp = { 0.0f, 1.0f, 0.0f };
	const float radius = MathVec3DLength(lightPos);

	const Vec3D right = (targetPos - lightPos).Cross(worldUp);
	const Vec3D up = right.Cross(targetPos - lightPos);

	view = MathMat4X4ViewAt(&lightPos, &targetPos, &up);
	proj = MathMat4X4OrthographicOffCenter(-radius, radius, -radius, radius,
					       m_Camera.GetZNear(),
					       m_Camera.GetZFar());
}

void Game::DrawActor(const Actor &actor)
{
	if (actor.IsVisible()) {
		m_Renderer.BindShaderResources(BindTargets::PixelShader,
					       actor.GetShaderResources(),
					       ACTOR_NUM_TEXTURES);
		m_PerObjectData.material = actor.GetMaterial();
		m_PerObjectData.world = actor.GetWorld();
		m_PerObjectData.worldInvTranspose =
			MathMat4X4Inverse(actor.GetWorld());
		GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
					 sizeof(PerObjectConstants),
					 &m_PerObjectData, m_PerObjectCB.Get());
		Mat4X4 view = {};
		Mat4X4 proj = {};
		BuildShadowTransform(view, proj);
		const Mat4X4 toLightSpace = actor.GetWorld() * view * proj;
		m_PerFrameData.shadowTransform = toLightSpace;
		GameUpdateConstantBuffer(m_DR->GetDeviceContext(),
					 sizeof(PerFrameConstants),
					 &m_PerFrameData, m_PerFrameCB.Get());
		m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
					      m_PerObjectCB.Get(), 0);
		m_Renderer.BindConstantBuffer(BindTargets::PixelShader,
					      m_PerObjectCB.Get(), 0);
		m_Renderer.BindConstantBuffer(BindTargets::VertexShader,
					      m_PerFrameCB.Get(), 1);
		m_Renderer.BindConstantBuffer(BindTargets::PixelShader,
					      m_PerFrameCB.Get(), 1);

		m_Renderer.SetIndexBuffer(actor.GetIndexBuffer(), 0);
		m_Renderer.SetVertexBuffer(
			actor.GetVertexBuffer(),
			/*m_shaderManager.GetStrides()*/ sizeof(Vertex), 0);

		m_Renderer.DrawIndexed(actor.GetNumIndices(), 0, 0);
	}
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_DR->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();
}

void Game::CreateWindowSizeDependentResources()
{
	auto const size = m_DR->GetOutputSize();
	const float aspectRatio = static_cast<float>(size.right) /
				  static_cast<float>(size.bottom);
	float fovAngleY = 45.0f;

	// portrait or snapped view.
	if (aspectRatio < 1.0f) {
		fovAngleY *= 2.0f;
	}

	m_Camera.SetFov(fovAngleY);
	m_Camera.SetViewDimensions(size.right, size.bottom);

	g_OffscreenRTV = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
					       m_DR->GetOutputSize().right,
					       m_DR->GetOutputSize().bottom,
					       m_DR->GetDevice());
	g_BrightessRTV = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
					       m_DR->GetOutputSize().right,
					       m_DR->GetOutputSize().bottom,
					       m_DR->GetDevice());
	g_BlurRTV = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
					       m_DR->GetOutputSize().right,
					       m_DR->GetOutputSize().bottom,
					       m_DR->GetDevice());
	g_BlurRTV2 = std::make_unique<Texture>(DXGI_FORMAT_B8G8R8A8_UNORM,
					       m_DR->GetOutputSize().right,
					       m_DR->GetOutputSize().bottom,
					       m_DR->GetDevice());
}
