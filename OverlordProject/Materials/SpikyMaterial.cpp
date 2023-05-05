#include "stdafx.h"
#include "SpikyMaterial.h"

SpikyMaterial::SpikyMaterial()
:Material(L"Effects/SpikyShader.fx")
{
}

void SpikyMaterial::SetColor(const XMFLOAT4& color) const
{
	SetVariable_Vector(L"gColorDiffuse", color);
}

void SpikyMaterial::SetLight(const XMFLOAT3& light) const
{
	SetVariable_Vector(L"m_LightDirection", light);
}

void SpikyMaterial::SetSpikeLength(const float& length) const
{
	SetVariable_Scalar(L"gSpikeLength", length);
}

void SpikyMaterial::InitializeEffectVariables()
{
}
