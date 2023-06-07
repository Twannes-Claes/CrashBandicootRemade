#include "stdafx.h"
#include "SpinParticle.h"

void SpinParticle::Initialize(const SceneContext&)
{
	ParticleEmitterSettings settings{};

	settings.velocity = { 0.f, 0.f,0.f };
	settings.minSize = 0.3f;
	settings.maxSize = 0.8f;
	settings.minEnergy = 0.5f;
	settings.maxEnergy = 1.f;
	settings.minScale = 0.5f;
	settings.maxScale = 0.1f;

	settings.minEmitterRadius = 0.75f;
	settings.maxEmitterRadius = 1.f;

	const auto pObject = AddChild(new GameObject);
	pObject->AddComponent(new ParticleEmitterComponent(L"Textures/Spark.png", settings,125));

	pObject->GetTransform()->Translate(m_Pos);
}

