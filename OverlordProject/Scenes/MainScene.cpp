#include "stdafx.h"
#include "MainScene.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/Level.h"

#include "Prefabs/Player.h"

#include "Materials/Post/PostPixel.h"
#include "Materials/Post/PostBlur.h"

void MainScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	AddChild(new Level(this));

	m_pPlayer = AddChild(new Player());

	m_pPostPixel = MaterialManager::Get()->CreateMaterial<PostPixel>();
	m_pPostBlur = MaterialManager::Get()->CreateMaterial<PostBlur>();

	AddPostProcessingEffect(m_pPostPixel);
	AddPostProcessingEffect(m_pPostBlur);

	m_pPostPixel->SetIsEnabled(true);
	m_pPostBlur->SetIsEnabled(true);

	m_pPostPixel->SetPixelResolution(m_PixelRes[0], m_PixelRes[1]);
}

void MainScene::Update()
{
	
}

void MainScene::OnGUI()
{
	m_pPlayer->DrawImGUI();

	bool isEnabled = m_pPostPixel->IsEnabled();
	ImGui::Checkbox("Pixel", &isEnabled);
	m_pPostPixel->SetIsEnabled(isEnabled);

	bool isEnabled2 = m_pPostBlur->IsEnabled();
	ImGui::Checkbox("Blur", &isEnabled2);
	m_pPostBlur->SetIsEnabled(isEnabled2);

	ImGui::SliderFloat2("Pixel resolution", m_PixelRes, 50, 600);

	m_pPostPixel->SetPixelResolution(m_PixelRes[0], m_PixelRes[1]);
}
