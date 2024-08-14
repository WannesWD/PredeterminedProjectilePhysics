#include "pch.h"
#include "Effect.h"
#include "Texture.h"
#include <cassert>


Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const std::string& techniqueName)
{
	m_pEffect = LoadEffect(pDevice, assetFile);

	m_pTechnique = m_pEffect->GetTechniqueByName(techniqueName.c_str());
	if (!GetTechnique()->IsValid())
		std::wcout << L"Texhnique not valid\n";

	m_pSamplerVariable = m_pEffect->GetVariableByName("gSampleState")->AsSampler();
	if (!m_pSamplerVariable->IsValid())
		std::wcout << L"m_pSamplerVariable not valid\n";
	SetSamplers(pDevice);

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"m_pMatworldViewProjVariable not valid\n";

	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldVariable->IsValid())
		std::wcout << L"m_pMatworldVariable not valid\n";

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable not valid\n";
	SetRasterizerState(pDevice);
}

Effect::~Effect()
{
	m_pTechnique->Release();
	m_pEffect->Release();
	for (ID3D11SamplerState* sampler : m_pSamplerStates)
	{
		sampler->Release();
	}
}

ID3DX11Effect* Effect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique()
{
	return m_pTechnique;
}

void Effect::SetWorldViewProjMatrix(const dae::Matrix& worldViewProj)
{
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&worldViewProj));
}

void Effect::SetWorldMatrix(const dae::Matrix& worldMatrix)
{
	m_pMatWorldVariable->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));
}

void Effect::SetFilterMethod(const FilterMethod& filterMethod)
{
	if (filterMethod < maxMethod)
	{
		m_CurrentMethod = filterMethod;
		m_pSamplerVariable->SetSampler(0, m_pSamplerStates[m_CurrentMethod]);
	}
}

void Effect::SetResourceMaps(Texture* pDiffuseMap)
{
	if(m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseMap->GetSRV());
}

ID3D11RasterizerState* Effect::GetRasterizerState() const
{
	return m_pRasterizerState[0];
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result{};
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect{};

	DWORD shaderFlags = 0;
#ifdef DEBUG || _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // DEBUG || _DEBUG

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i{ 0 }; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath:" << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

void Effect::SetSamplers(ID3D11Device* pDevice)
{
	D3D11_SAMPLER_DESC pDesc{};
	pDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	pDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pDevice->CreateSamplerState(&pDesc, &m_pSamplerStates[Anisotropic]);

	pDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pDevice->CreateSamplerState(&pDesc, &m_pSamplerStates[Point]);

	pDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	pDevice->CreateSamplerState(&pDesc, &m_pSamplerStates[Linear]);
	
	m_pSamplerVariable->SetSampler(0, m_pSamplerStates[Anisotropic]);
}

void Effect::SetRasterizerState(ID3D11Device* pDevice)
{
	D3D11_RASTERIZER_DESC pDesc{};
	pDesc.FillMode = D3D11_FILL_SOLID;
	pDesc.CullMode = D3D11_CULL_BACK;
	
	HRESULT result{ pDevice->CreateRasterizerState(&pDesc, &m_pRasterizerState[0])};
}

