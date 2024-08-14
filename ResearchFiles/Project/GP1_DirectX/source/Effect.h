#pragma once
#include <array>

class Texture;
class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const std::string& techniqueName = "DefaultTechnique");
	virtual ~Effect();

	Effect(const Effect& other) = delete;
	Effect(Effect&& other) noexcept = delete;
	Effect& operator=(const Effect& other) = delete;
	Effect& operator=(Effect&& other) noexcept = delete;

	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique();
	void SetWorldViewProjMatrix(const dae::Matrix& worldViewProj);
	void SetWorldMatrix(const dae::Matrix& worldMatrix);
	ID3D11RasterizerState* GetRasterizerState() const;

	enum FilterMethod
	{
		Anisotropic,
		Point,
		Linear,

		maxMethod
	};
	void SetFilterMethod(const FilterMethod& filterMethod);
protected:
	ID3DX11Effect* m_pEffect{};
	void SetResourceMaps(Texture* pDiffuseMap);
	virtual void SetRasterizerState(ID3D11Device* pDevice);
	std::array<ID3D11RasterizerState*, 1> m_pRasterizerState{};
private:
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	void SetSamplers(ID3D11Device* pDevice);
	const int m_NumTechniques{ 3 };
	int m_CurrentTechnique{ 0 };

	std::array<ID3D11SamplerState*, 3> m_pSamplerStates{};
	ID3DX11EffectTechnique* m_pTechnique;

	ID3DX11EffectSamplerVariable* m_pSamplerVariable{};

	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable{};

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};

	FilterMethod m_CurrentMethod{ Anisotropic };
};

