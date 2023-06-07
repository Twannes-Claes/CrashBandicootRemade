#include "stdafx.h"
#include "WumpaHUD.h"

void WumpaHUD::Initialize(const SceneContext& sceneContext)
{
	m_pFont = ContentManager::Load<SpriteFont>(L"Fonts/CrashFont_70.fnt");
	m_pFontOutline = ContentManager::Load<SpriteFont>(L"Fonts/CrashFont_72.fnt");

	m_Pos = XMFLOAT2(sceneContext.windowWidth - 250, 50);

	const auto pHUDObject = AddChild(new GameObject);

	pHUDObject->AddComponent(new SpriteComponent(L"Textures/WumpaHUD.png", XMFLOAT2(0.5f, 0.5f)));

	m_pSpriteTransform = pHUDObject->GetTransform();

	m_pSpriteTransform->Scale(0.25f, 0.25f, 0.25f);
	m_pSpriteTransform->Translate(sceneContext.windowWidth - 80, 80, 0);
}

void WumpaHUD::Update(const SceneContext& sceneContext)
{
	m_Rotation += sceneContext.pGameTime->GetElapsed() * m_Speed;

	if (abs(m_Rotation) >= m_MaxAngle) m_Speed *= -1;

	m_pSpriteTransform->Rotate(0, 0, m_Rotation);
}

void WumpaHUD::PostInitialize(const SceneContext&)
{
	m_Text = std::to_wstring(m_AmountWumpas) + L"/" + std::to_wstring(m_TotalWUmpas);
}

void WumpaHUD::PostDraw(const SceneContext&)
{
	TextRenderer::Get()->DrawText(m_pFontOutline, m_Text, m_Pos, XMFLOAT4(Colors::Orange));
	TextRenderer::Get()->DrawText(m_pFont, m_Text, m_Pos, XMFLOAT4(Colors::Black));
}
