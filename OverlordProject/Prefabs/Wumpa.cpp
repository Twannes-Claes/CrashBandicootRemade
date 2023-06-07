#include "stdafx.h"
#include "Wumpa.h"

#include "WumpaHUD.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

Wumpa::Wumpa(const XMFLOAT3& pos)
:m_SpawnPos(pos)
{
	const auto pFmod{ SoundManager::Get()->GetSystem() };
	std::stringstream filePath{};
	filePath << "Resources/Sounds/Fruit.mp3";
	const auto result = pFmod->createStream(filePath.str().c_str(), FMOD_DEFAULT, nullptr, &m_pBreakSound);

	SoundManager::Get()->ErrorCheck(result);

	m_pChannel->setVolume(0.7f);
}

void Wumpa::Initialize(const SceneContext& )
{
	m_pMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	m_pObject = AddChild(new GameObject());

	const auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMat->SetDiffuseTexture(L"Textures/Wumpa.dds");

	const auto pModel = m_pObject->AddComponent(new ModelComponent(L"Meshes/Wumpa.ovm"));
	pModel->SetMaterial(pMat);

	m_pObject->GetTransform()->Scale(0.025f, 0.025f, 0.025f);

	m_pRigid = m_pObject->AddComponent(new RigidBodyComponent());

	m_pRigid->AddCollider(PxBoxGeometry(0.3f, 0.3f, 0.3f), *m_pMat);

	m_pObject->GetTransform()->Translate(m_SpawnPos.x, m_SpawnPos.y, m_SpawnPos.z);

	//attach the trigger volume to the wumpa
	m_pRigid->AddCollider(PxBoxGeometry(0.5f, 0.5f, 0.5f), *m_pMat, true);

	m_pRigid->SetCollisionGroup(CollisionGroup::Group8);
	m_pRigid->SetCollisionIgnoreGroups(~(CollisionGroup::Group9 | CollisionGroup::Group0));

	m_pObject->SetOnTriggerCallBack([this](GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction action)
		{
			OnTriggerEvent(pTriggerObject, pOtherObject, action);
		});

	m_pRigid->AddForce(XMFLOAT3(0, 5, 0), PxForceMode::eIMPULSE);
}

void Wumpa::Update(const SceneContext&)
{
	if(m_CanDelete)
	{
		WumpaHUD::AddWumpa();

		const auto pFmod{ SoundManager::Get()->GetSystem() };

		const auto result = pFmod->playSound(m_pBreakSound, nullptr, false, &m_pChannel);

		SoundManager::Get()->ErrorCheck(result);

		GetParent()->RemoveChild(this, true);
	}
}

void Wumpa::OnTriggerEvent(GameObject*, GameObject*, PxTriggerAction action)
{
	if (action == PxTriggerAction::ENTER)
	{
		m_CanDelete = true;
	}
}
