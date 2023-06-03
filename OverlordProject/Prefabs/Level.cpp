#include "stdafx.h"
#include "Level.h"

#include "Crate.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

Level::Level(GameScene* scene)
{
	m_pGameScene = scene;
}

void Level::Initialize(const SceneContext&)
{
	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*m_pGameScene, pDefaultMaterial);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/SimpleLevel.ovm"));
	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/SimpleLevel.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ .5f,.5f,.5f })), *pDefaultMaterial);
	pLevelObject->GetTransform()->Scale(.5f, .5f, .5f);

	pLevelActor->SetCollisionGroup(CollisionGroup::Group0);

	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");

	//Ground Mesh
	//***********
	const auto pGroundObj = AddChild(new GameObject());
	const auto pGroundModel = pGroundObj->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	pGroundModel->SetMaterial(pGroundMaterial);
	
	pGroundObj->GetTransform()->Scale(10.0f, 10.0f, 10.0f);

	//CRATES
	//******
	const float cratesize = Crate::GetCrateSize();

	SpawnCrate(0, cratesize /2, -20, false);
	SpawnCrate(0, cratesize * 1.5f, -20, true);
	SpawnCrate(0, cratesize * 1.5f, -20, true);
	SpawnCrate(0, cratesize * 1.5f, -25, true);
	SpawnCrate(0, cratesize * 1.5f, -30, true);
	SpawnCrate(0, cratesize * 1.5f, -35, true);
	SpawnCrate(0, cratesize * 1.5f, -40, true);

	SpawnCrate(5, cratesize * 1.5f, -20, true);
	SpawnCrate(5, cratesize * 1.5f, -20, true);
	SpawnCrate(5, cratesize * 1.5f, -25, true);
	SpawnCrate(5, cratesize * 1.5f, -30, true);
	SpawnCrate(5, cratesize * 1.5f, -35, true);
	SpawnCrate(5, cratesize * 1.5f, -40, true);
}

void Level::SpawnCrate(float x, float y, float z, bool hasWumpa)
{
	const auto crate = AddChild(new Crate(x,y,z, hasWumpa));

	crate->GetTransform()->Translate(0, 0, 0);
}
