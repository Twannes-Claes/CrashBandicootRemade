#include "stdafx.h"
#include "Crate.h"

#include "Player.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

Crate::Crate(float x, float y, float z, bool hasWumpa)
:m_HasWumpa(hasWumpa),
m_Position(PxVec3(x, y, z))
{}

void Crate::Initialize(const SceneContext& /*sceneContext*/)
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	const auto pObject = AddChild(new GameObject());

	const auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();

	std::wstring path{};

	switch(m_HasWumpa)
	{
		case true:
			path = L"Textures/Crate_Wumpa.dds";
			break;
		case false:
			path = L"Textures/Crate.dds";
			break;
	}

	pMat->SetDiffuseTexture(path);

	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Crate.ovm"));
	pModel->SetMaterial(pMat);

	const auto pRigid = pObject->AddComponent(new RigidBodyComponent(true));
	const auto pConvex = ContentManager::Load<PxConvexMesh>(L"Meshes/Crate.ovpc");
	pRigid->AddCollider(PxConvexMeshGeometry(pConvex, PxMeshScale({ m_Scale,m_Scale,m_Scale })), *pDefaultMaterial);

	pRigid->AddCollider(PxBoxGeometry(1.75f, 1.75f, 1.75f), *pDefaultMaterial, true);

	pRigid->SetCollisionGroup(CollisionGroup::Group8);
	pRigid->SetCollisionIgnoreGroups(~CollisionGroup::Group9);

	pObject->SetOnTriggerCallBack([this](GameObject* self, GameObject* triggeredObject, PxTriggerAction action)
		{
			OnTriggerEvent(self, triggeredObject, action);
		});

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
