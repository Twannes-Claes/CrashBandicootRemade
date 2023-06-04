#include "stdafx.h"
#include "PostPixel.h"



PostPixel::PostPixel():
PostProcessingMaterial(L"Effects/Post/PixelatedShader.fx")
{
}

void PostPixel::SetPixelResolution(float x, float y) const
{
	const float floats[2]{ x, y };
	m_pBaseEffect->GetVariableByName("gResolution")->AsVector()->SetFloatVector(floats);
}


