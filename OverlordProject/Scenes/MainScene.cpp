#include "stdafx.h"
#include "MainScene.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/Level.h"

#include "Prefabs/Player.h"

void MainScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	AddChild(new Level(this));

	m_pPlayer = AddChild(new Player());
}

void MainScene::Update()
{
	
}

void MainScene::OnGUI()
{
	m_pPlayer->DrawImGUI();
}
