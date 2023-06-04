#include "stdafx.h"
#include "Wumpa.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

Wumpa::Wumpa(const XMFLOAT3& pos)
:m_SpawnPos(pos)
{
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

	m_pRigid->AddForce(XMFLOAT3(0, 5, 0), PxForceMode::eIMPULSE);
}

void Wumpa::Update(const SceneContext& sceneContext)
{
	//add a short delay to pick up the wumpa
	m_CollisionTimer -= sceneContext.pGameTime->GetElapsed();

	//when delay is over you can add the collider
	if (m_CollisionTimer <= 0 && m_CanAddCollider == -1) m_CanAddCollider = 1;

	if(m_CanDelete)
	{
		GetParent()->RemoveChild(this, true);
	}
	if(m_CanAddCollider == 1)
	{
		//attach the trigger volume to the wumpa
		m_pRigid->AddCollider(PxBoxGeometry(0.5f, 0.5f, 0.5f), *m_pMat, true);

		m_pRigid->SetCollisionGroup(CollisionGroup::Group8);
		m_pRigid->SetCollisionIgnoreGroups(~CollisionGroup::Group9);

		m_pObject->SetOnTriggerCallBack([this](GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction action)
		{
			OnTriggerEvent(pTriggerObject, pOtherObject, action);
		});

		//mark the collider as added
		m_CanAddCollider = 0;
	}
}

void Wumpa::OnTriggerEvent(GameObject*, GameObject*, PxTriggerAction action)
{
	if (action == PxTriggerAction::ENTER)
	{
		m_CanDelete = true;
	}
}
