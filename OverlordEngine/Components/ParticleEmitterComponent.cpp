#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	TODO_W9(L"Implement Destructor")

	delete[] m_ParticlesArray;

	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Initialize")

	m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();

	CreateVertexBuffer(sceneContext);

	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement CreateVertexBuffer")

	if (m_pVertexBuffer) SafeRelease(m_pVertexBuffer);

	//*************
	//VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBuffDesc{};
	vertexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBuffDesc.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	vertexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	vertexBuffDesc.MiscFlags = 0;

	const HRESULT result = sceneContext.d3dContext.pDevice->CreateBuffer(&vertexBuffDesc, nullptr, &m_pVertexBuffer);

	HANDLE_ERROR(result);
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Update")

	const float elapsed = sceneContext.pGameTime->GetElapsed();

	const float particleInterval =  ((m_EmitterSettings.minEnergy + m_EmitterSettings.maxEnergy) / 2) / m_ParticleCount;

	m_LastParticleSpawn += elapsed;

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE pSubResource;

	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pSubResource);

	VertexParticle* pBuffer = static_cast<VertexParticle*>(pSubResource.pData);

	for(unsigned int i{}; i < m_ParticleCount; ++i)
	{
		Particle& currP = m_ParticlesArray[i];

		if (currP.isActive) UpdateParticle(currP, elapsed);

		if (!currP.isActive && m_LastParticleSpawn >= particleInterval) SpawnParticle(currP);

		if(currP.isActive)
		{
			pBuffer[m_ActiveParticles] = currP.vertexInfo;

			++m_ActiveParticles;
		}
	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	TODO_W9(L"Implement UpdateParticle")

	if(!p.isActive) return;

	p.currentEnergy -= elapsedTime;

	if (p.currentEnergy < 0) { p.isActive = false; return; }

	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&p.vertexInfo.Position) + XMLoadFloat3(&m_EmitterSettings.velocity) * elapsedTime);

	const float lifePercent = p.currentEnergy / p.totalEnergy;

	constexpr float delay = 2.f;

	p.vertexInfo.Color.w = m_EmitterSettings.color.w * lifePercent * delay;

	p.vertexInfo.Size = p.initialSize * powf(p.sizeChange, 1.f - lifePercent);
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	TODO_W9(L"Implement SpawnParticle")

	p.isActive = true;

	p.currentEnergy = p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);

	//constexpr XMFLOAT3 randomDir{1, 0, 0};
	//
	//const auto matrix= XMMatrixRotationRollPitchYaw
	//(
	//	MathHelper::randF(-XM_PI, XM_PI),
	//	MathHelper::randF(-XM_PI, XM_PI),
	//	MathHelper::randF(-XM_PI, XM_PI)
	//);
	//
	//XMVector3TransformNormal(XMLoadFloat3(&randomDir), matrix);

	const XMVECTOR randomDir
	{
		XMVector3Normalize(
		{
			MathHelper::randF(-1.0f, 1.0f),
			MathHelper::randF(-1.0f, 1.0f),
			MathHelper::randF(-1.0f, 1.0f)
		})
	};

	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&GetTransform()->GetWorldPosition()) + randomDir * MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius));

	p.vertexInfo.Size = p.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement PostDraw")

	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());

	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	auto& technique = m_pParticleMaterial->GetTechniqueContext();

	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(technique.pInputLayout);

	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	constexpr UINT stride = sizeof(VertexParticle);

	constexpr UINT offset = 0;

	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc{};

	technique.pTechnique->GetDesc(&techDesc);

	for(UINT i{}; i < techDesc.Passes; ++i)
	{
		technique.pTechnique->GetPassByIndex(i)->Apply(0, sceneContext.d3dContext.pDeviceContext);
		sceneContext.d3dContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}