#include "pch.h"
#include "ShadeEffect.h"
#include "Texture.h"

ShadeEffect::ShadeEffect(ID3D11Device* pDevice, const std::wstring& assetFile,
	Texture* pDiffuseMap, Texture* pNormalMap, Texture* pSpecularMap, Texture* pGlossMap) :
	Effect(pDevice, assetFile)
{
	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"m_pNormalMapVariable not valid\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"m_pSpecularMapVariable not valid\n";

	m_pGlossMapVariable = m_pEffect->GetVariableByName("gGlossMap")->AsShaderResource();
	if (!m_pGlossMapVariable->IsValid())
		std::wcout << L"m_pGlossMapVariable not valid\n";

	m_pLightDirectionVariable = m_pEffect->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pLightDirectionVariable->IsValid())
		std::wcout << L"m_pLightDirectionVariable not valid\n";
	m_pLightDirectionVariable->SetFloatVector(m_LightDirection);

	m_pCameraPositionVariable = m_pEffect->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraPositionVariable->IsValid())
		std::wcout << L"m_pCameraPositionVariable not valid\n";

	m_pLightColorVariable = m_pEffect->GetVariableByName("gLightColor")->AsVector();
	if (!m_pLightColorVariable->IsValid())
		std::wcout << L"m_pLightColorVariable not valid\n";
	m_pLightColorVariable->SetFloatVector(m_LightColor);

	m_pLightIntensityVariable = m_pEffect->GetVariableByName("gLightIntensity")->AsScalar();
	if (!m_pLightIntensityVariable->IsValid())
		std::wcout << L"m_pLightIntensityVariable not valid\n";
	m_pLightIntensityVariable->SetFloat(7.0f);

	m_pShininessVariable = m_pEffect->GetVariableByName("gShininess")->AsScalar();
	if (!m_pShininessVariable->IsValid())
		std::wcout << L"m_pShininessVariable not valid\n";
	m_pShininessVariable->SetFloat(25.f);
	SetResourceMaps(pDiffuseMap, pNormalMap, pSpecularMap, pGlossMap);
}

ShadeEffect::~ShadeEffect()
{
	m_pNormalMapVariable->Release();
	m_pSpecularMapVariable->Release();
	m_pGlossMapVariable->Release();

	m_pLightDirectionVariable->Release();
	m_pCameraPositionVariable->Release();
	m_pLightColorVariable->Release();

	m_pLightIntensityVariable->Release();
	m_pShininessVariable->Release();
}

void ShadeEffect::SetResourceMaps(Texture* pDiffuseMap, Texture* pNormalMap, Texture* pSpecularMap, Texture* pGlossMap)
{
	Effect::SetResourceMaps(pDiffuseMap);

	if (m_pNormalMapVariable)
		m_pNormalMapVariable->SetResource(pNormalMap->GetSRV());

	if (m_pSpecularMapVariable)
		m_pSpecularMapVariable->SetResource(pSpecularMap->GetSRV());

	if (m_pGlossMapVariable)
		m_pGlossMapVariable->SetResource(pGlossMap->GetSRV());
}
