#include "pch.h"
#include "SimpleEffect.h"

SimpleEffect::SimpleEffect(ID3D11Device* pDevice, const std::wstring& assetFile, Texture* pDiffuseMap) :
	Effect(pDevice, assetFile, "SimpleTechnique")
{
	Effect::SetResourceMaps(pDiffuseMap);
	SetRasterizerState(pDevice);
}

void SimpleEffect::SetRasterizerState(ID3D11Device* pDevice)
{
	D3D11_RASTERIZER_DESC pDesc{};
	pDesc.FillMode = D3D11_FILL_SOLID;
	pDesc.CullMode = D3D11_CULL_BACK;

	pDevice->CreateRasterizerState(&pDesc, &m_pRasterizerState[0]);
}
