#include "pch.h"
#include "PartialCoverageEffect.h"

PartialCoverageEffect::PartialCoverageEffect(ID3D11Device* pDevice, const std::wstring& assetFile, Texture* pDiffuseMap) :
	Effect(pDevice,assetFile,"PartialCoverageTechnique")
{
	Effect::SetResourceMaps(pDiffuseMap);
	SetRasterizerState(pDevice);
}

void PartialCoverageEffect::SetRasterizerState(ID3D11Device* pDevice)
{
	D3D11_RASTERIZER_DESC pDesc{};
	pDesc.FillMode = D3D11_FILL_SOLID;
	pDesc.CullMode = D3D11_CULL_NONE;

	pDevice->CreateRasterizerState(&pDesc, &m_pRasterizerState[0]);
}
