#include "stdafx.h"
#include "PostBlur.h"

PostBlur::PostBlur() :
PostProcessingMaterial(L"Effects/Post/Blur.fx")
{
}

void PostBlur::SetOffset(float n) const
{
	m_pBaseEffect->GetVariableByName("gOffset")->AsScalar()->SetFloat(n);
}

void PostBlur::SetSamples(int n) const
{
	m_pBaseEffect->GetVariableByName("gAmountSamples")->AsScalar()->SetInt(n);
}


