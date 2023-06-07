#include "stdafx.h"
#include "Crate.h"

#include "Level.h"
#include "Player.h"
#include "Wumpa.h"
#include "WumpaHUD.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Particles/CrateParticle.h"
#include "Utils/FmodHelper.h"

Crate::Crate(const XMFLOAT3& pos, const CrateState& state, Level* pLevel)
:m_State(state),
m_Position(pos),
m_pLevel(pLevel)
{
	const auto pFmod{ SoundManager::Get()->GetSystem() };
	std::stringstream filePath{};
	filePath << "Resources/Sounds/Crate.mp3";
	const auto result = pFmod->createStream(filePath.str().c_str(), FMOD_DEFAULT, nullptr, &m_pBreakSound);

	SoundManager::Get()->ErrorCheck(result);

	m_pChannel->setVolume(1.f);
}

void Crate::Initialize(const SceneContext& /*sceneContext*/)
{
	//create default mat
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//make main object
	const auto pObject = AddChild(new GameObject());

	//make shadow mat
	const auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();

	//assign correct texture
	std::wstring path{};

	switch(m_State)
	{
		case CrateState::Fruit:
			path = L"Textures/Crate_Wumpa.dds";
			WumpaHUD::AddTotalWumpas();
			break;
		case CrateState::Normal:
			path = L"Textures/Crate.dds";
			break;
		case CrateState::Metal:
			path = L"Textures/MetalCrate.dds";
			break;
	}

	pMat->SetDiffuseTexture(path);

	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Crate.ovm"));
	pModel->SetMaterial(pMat);

	const auto pRigid = pObject->AddComponent(new RigidBodyComponent(true));
	const auto pConvex = ContentManager::Load<PxConvexMesh>(L"Meshes/Crate.ovpc");
	pRigid->AddCollider(PxConvexMeshGeometry(pConvex, PxMeshScale({ m_Scale,m_Scale,m_Scale })), *pDefaultMaterial);

	if (m_State != CrateState::Metal)
	{
		pRigid->AddCollider(PxBoxGeometry(1.75f, 1.75f, 1.75f), *pDefaultMaterial, true);

		pRigid->SetCollisionGroup(CollisionGroup::Group8);
		pRigid->SetCollisionIgnoreGroups(~CollisionGroup::Group9);

		pObject->SetOnTriggerCallBack([this](GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction action)
			{
				OnTriggerEvent(pTriggerObject, pOtherObject, action);
			});
	}

	pObject->GetTransform()->Scale(m_Scale, m_Scale, m_Scale);
	pObject->GetTransform()->Translate(m_Position.x, m_Position.y, m_Position.z);
}

void Crate::Update(const SceneContext& /*sceneContext*/)
{
	if(m_PlayerInside)
	{
		//std::cout << "Player Inside\n";

		if(Player::IsPlayerAttacking())
		{
			//std::cout << "Player Inside attacking\n";
			if (m_State == CrateState::Fruit)
			{
				SpawnWumpa();
			}

			const auto pFmod{ SoundManager::Get()->GetSystem() };

			const auto result = pFmod->playSound(m_pBreakSound, nullptr, false, &m_pChannel);

			SoundManager::Get()->ErrorCheck(result);


			m_pLevel->AddChild(new CrateParticle(m_Position));
			GetParent()->RemoveChild(this, true);
		}
	}
}

void Crate::OnTriggerEvent(GameObject* , GameObject* , PxTriggerAction action)
{
	if(action == PxTriggerAction::ENTER)
	{
		m_PlayerInside = true;
	}
	else if(action == PxTriggerAction::LEAVE)
	{
		m_PlayerInside = false;
	}
}

void Crate::SpawnWumpa() const
{
	const auto pWumpa = m_pLevel->AddChild(new Wumpa(m_Position));
	pWumpa->GetTransform()->Translate(0, 0, 0);
}
