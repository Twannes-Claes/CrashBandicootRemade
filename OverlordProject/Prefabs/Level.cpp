#include "stdafx.h"
#include "Level.h"

#include "Crate.h"
#include "Wumpa.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/SkyboxMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

Level::Level(GameScene* scene)
{
	m_pGameScene = scene;

	//sounds

	const auto pFmod{ SoundManager::Get()->GetSystem() };
	std::stringstream filePath{};
	filePath << "Resources/Sounds/Music.mp3";
	const auto result = pFmod->createStream(filePath.str().c_str(), FMOD_2D | FMOD_LOOP_NORMAL, nullptr, &m_pMusic);

	SoundManager::Get()->ErrorCheck(result);

	m_pChannel->setVolume(0.01f);
}

void Level::Initialize(const SceneContext& sceneContext)
{
	sceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*m_pGameScene, pDefaultMaterial);

	//Add the volcano level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/Level.ovm"));


	//make the material
	const auto pLevelMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pLevelMat->SetDiffuseTexture(L"Textures/LevelTexture.png");

	pLevelMesh->SetMaterial(pLevelMat);

	//make the collision
	const auto pLevelRigid = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Level.ovpt");
	pLevelRigid->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 1.5f,1.5f,1.5f })), *pDefaultMaterial);
	pLevelObject->GetTransform()->Scale(1.5f, 1.5f, 1.5f);

	pLevelRigid->SetCollisionGroup(CollisionGroup::Group0);

	//add border walls
	AddBorderWalls();

	//CRATES
	//******
	const float cratesize = Crate::GetCrateSize();

	float y = cratesize / 2;

	SpawnCrate(-10, y + 31 + cratesize, -15,Crate::CrateState::Fruit);
	SpawnCrate(-10, y + 31 + cratesize * 2, -15,Crate::CrateState::Fruit);
	SpawnCrate(-10, y + 31 + cratesize * 3, -15,Crate::CrateState::Fruit);

	SpawnCrate(-90, y + 13 + cratesize, -90, Crate::CrateState::Fruit);
	SpawnCrate(-95, y + 20 + cratesize, 34, Crate::CrateState::Fruit);
	SpawnCrate(-5, y + 13 + cratesize, -90, Crate::CrateState::Fruit);
	SpawnCrate(-30, y + 31 + cratesize, -20, Crate::CrateState::Fruit);

	SpawnCrate(0, y + 41 + cratesize, 30, Crate::CrateState::Fruit);
	SpawnCrate(0, y + 41 + cratesize * 2, 30, Crate::CrateState::Fruit);

	SpawnCrate(0 + cratesize, y + 41 + cratesize, 30, Crate::CrateState::Fruit);
	SpawnCrate(0 + cratesize, y + 41 + cratesize * 2, 30, Crate::CrateState::Fruit);
	//
	SpawnCrate(0, y + 41 + cratesize, 30 + cratesize, Crate::CrateState::Fruit);
	SpawnCrate(0, y + 41 + cratesize * 2, 30 + cratesize, Crate::CrateState::Fruit);

	SpawnCrate(0 + cratesize, y + 41 + cratesize, 30 + cratesize, Crate::CrateState::Fruit);
	SpawnCrate(0 + cratesize, y + 41 + cratesize * 2, 30 + cratesize, Crate::CrateState::Fruit);
	//
	SpawnCrate(-10 - cratesize, y + 31, -15 -cratesize, Crate::CrateState::Metal);
	SpawnCrate(-10 - cratesize, y + 31, -15, Crate::CrateState::Metal);
	SpawnCrate(-10 + cratesize, y + 31, -15 -cratesize, Crate::CrateState::Metal);
	SpawnCrate(-10 + cratesize, y + 31, -15 + cratesize, Crate::CrateState::Metal);
	SpawnCrate(-10 + cratesize, y + 31, -15, Crate::CrateState::Metal);
	SpawnCrate(-10, y + 31, -15 -cratesize, Crate::CrateState::Metal);
	SpawnCrate(-10, y + 31, -15 +cratesize, Crate::CrateState::Metal);
	SpawnCrate(-10 - cratesize, y + 31, -15 +cratesize, Crate::CrateState::Metal);

	y += cratesize;

	SpawnCrate(-10 - cratesize, y + 31, -15 - cratesize, Crate::CrateState::Normal);
	SpawnCrate(-10 - cratesize, y + 31, -15, Crate::CrateState::Normal);
	SpawnCrate(-10 + cratesize, y + 31, -15 - cratesize, Crate::CrateState::Normal);
	SpawnCrate(-10 + cratesize, y + 31, -15 + cratesize, Crate::CrateState::Normal);
	SpawnCrate(-10 + cratesize, y + 31, -15, Crate::CrateState::Normal);
	SpawnCrate(-10, y + 31, -15 - cratesize, Crate::CrateState::Normal);
	SpawnCrate(-10, y + 31, -15 + cratesize, Crate::CrateState::Normal);
	SpawnCrate(-10 - cratesize, y + 31, -15 + cratesize, Crate::CrateState::Normal);


	//SKYBOX
	//******
	const auto pSkyboxObject = AddChild(new GameObject);

	const auto pSkyBoxMat = MaterialManager::Get()->CreateMaterial<SkyboxMaterial>();
	pSkyBoxMat->SetDiffuseTexture(L"Textures/SkyboxOwn.dds");

	pSkyboxObject->AddComponent(new ModelComponent( L"Meshes/Box.ovm", false ))->SetMaterial(pSkyBoxMat);

	pSkyboxObject->GetTransform()->Scale(100.0f);

	const auto pFmod{ SoundManager::Get()->GetSystem() };

	const auto result = pFmod->playSound(m_pMusic, nullptr, false, &m_pChannel);

	SoundManager::Get()->ErrorCheck(result);
}

void Level::SpawnCrate(float x, float y, float z, Crate::CrateState state)
{
	const auto crate = AddChild(new Crate(XMFLOAT3(x,y,z), state, this));

	crate->GetTransform()->Translate(0, 0, 0);
}

void Level::AddBorderWalls()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	constexpr int wallWidth = 150;
	constexpr int wallHeight = 50;

	for (int i = 0; i < 4; ++i)
	{
		const auto pWall = AddChild(new GameObject);
		const auto pWallRigid = pWall->AddComponent(new RigidBodyComponent(true));
		pWallRigid->AddCollider(PxBoxGeometry(wallWidth, wallHeight, 1), *pDefaultMaterial);

		//Position the walls on the right sider
		switch (i)
		{
			case 0: // Front
				pWall->GetTransform()->Translate(0, wallHeight, wallWidth);
				break;
			case 1: // Back
				pWall->GetTransform()->Translate(0, wallHeight, -wallWidth);
				break;
			case 2: // Left
				pWall->GetTransform()->Translate(-wallWidth, wallHeight, 0);
				pWall->GetTransform()->Rotate(0, 90, 0);
				break;
			case 3: // Right
				pWall->GetTransform()->Translate(wallWidth, wallHeight, 0);
				pWall->GetTransform()->Rotate(0, 90, 0);
				break;
			default: ;
		}
	}
}


