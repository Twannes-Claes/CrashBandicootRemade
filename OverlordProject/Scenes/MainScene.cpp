#include "stdafx.h"
#include "MainScene.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/Level.h"

#include "Prefabs/Player.h"

#include "Materials/Post/PostPixel.h"

void MainScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	AddChild(new Level(this));

	m_pPlayer = AddChild(new Player());

	m_pPostPixel = MaterialManager::Get()->CreateMaterial<PostPixel>();

	AddPostProcessingEffect(m_pPostPixel);

	//m_pPostPixel->SetIsEnabled(true);
}

void MainScene::Update()
{
	
}

void MainScene::OnGUI()
{
	m_pPlayer->DrawImGUI();

	bool isEnabled = m_pPostPixel->IsEnabled();
	ImGui::Checkbox("Blur", &isEnabled);
	m_pPostPixel->SetIsEnabled(isEnabled);
}
