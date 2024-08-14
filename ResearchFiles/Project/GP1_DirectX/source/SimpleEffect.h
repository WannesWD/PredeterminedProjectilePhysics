#pragma once
#include "Effect.h"

class SimpleEffect final : public Effect
{
public:
	SimpleEffect(ID3D11Device* pDevice, const std::wstring& assetFile, Texture* pDiffuseMap);
	virtual ~SimpleEffect() = default;

	SimpleEffect(const SimpleEffect& other) = delete;
	SimpleEffect(SimpleEffect&& other) noexcept = delete;
	SimpleEffect& operator=(const SimpleEffect& other) = delete;
	SimpleEffect& operator=(SimpleEffect&& other) noexcept = delete;

private:
	void SetRasterizerState(ID3D11Device* pDevice) override;
};


