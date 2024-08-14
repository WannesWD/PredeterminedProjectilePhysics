#pragma once
#include "Effect.h"

class Texture;
class ShadeEffect final : public Effect
{
public:
	ShadeEffect(ID3D11Device* pDevice, const std::wstring& assetFile, 
		Texture* pDiffuseMap, Texture* pNormalMap, Texture* pSpecularMap, Texture* pGlossMap);
	virtual ~ShadeEffect() override;

	ShadeEffect(const ShadeEffect& other) = delete;
	ShadeEffect(ShadeEffect&& other) noexcept = delete;
	ShadeEffect& operator=(const ShadeEffect& other) = delete;
	ShadeEffect& operator=(ShadeEffect&& other) noexcept = delete;
private:
	void SetResourceMaps(Texture* pDiffuseTexture, Texture* pNormalMap, Texture* pSpecularMap, Texture* m_pGlossMap);
	
	static const int vectorSize{ 4 };
	const float m_LightDirection[vectorSize]{ .577f,-.577f,.577f,0.f };
	const float m_LightColor[vectorSize]{ .6f,.6f,.6f,0.f };

	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pGlossMapVariable{};

	ID3DX11EffectVectorVariable* m_pLightDirectionVariable{};
	ID3DX11EffectVectorVariable* m_pCameraPositionVariable{};
	ID3DX11EffectVectorVariable* m_pLightColorVariable{};

	ID3DX11EffectScalarVariable* m_pLightIntensityVariable{};
	ID3DX11EffectScalarVariable* m_pShininessVariable{};
};

