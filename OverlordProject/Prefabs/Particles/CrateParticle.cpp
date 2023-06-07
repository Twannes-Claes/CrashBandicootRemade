#include "stdafx.h"
#include "CrateParticle.h"

#include "Prefabs/Crate.h"

void CrateParticle::Initialize(const SceneContext&)
{
	ParticleEmitterSettings settings{};

	settings.velocity = { 0.f,-1.5f,0.f };
	settings.minSize = 0.3f;
	settings.maxSize = 0.8f;
	settings.minEnergy = m_LifeTime;
	settings.maxEnergy = 2.f;
	settings.minScale = 0.5f;
	settings.maxScale = 0.1f;

	settings.minEmitterRadius = 0.f;
	settings.maxEmitterRadius = 1.5f;

	const auto pObject = AddChild(new GameObject);
	m_pEmitter = pObject->AddComponent(new ParticleEmitterComponent(L"Textures/Dust.png", settings, 50));

	//const XMFLOAT3 correctPos = XMFLOAT3(m_Pos.x - 1.75f, m_Pos.y, m_Pos.z);

	pObject->GetTransform()->Translate(m_Pos);
}

void CrateParticle::Update(const SceneContext& sceneContext)
{
	m_CurrentLifeTime += sceneContext.pGameTime->GetElapsed();

	if (m_CurrentLifeTime >= m_LifeTime)
	{
		GetParent()->RemoveChild(this, true);
	}
}
