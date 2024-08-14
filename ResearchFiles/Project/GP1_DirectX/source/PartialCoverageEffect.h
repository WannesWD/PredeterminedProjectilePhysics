#pragma once
#include "Effect.h"

class PartialCoverageEffect final : public Effect
{
public:
	PartialCoverageEffect(ID3D11Device* pDevice, const std::wstring& assetFile, Texture* pDiffuseMap);
	virtual ~PartialCoverageEffect() = default;

	PartialCoverageEffect(const PartialCoverageEffect& other) = delete;
	PartialCoverageEffect(PartialCoverageEffect&& other) noexcept = delete;
	PartialCoverageEffect& operator=(const PartialCoverageEffect& other) = delete;
	PartialCoverageEffect& operator=(PartialCoverageEffect&& other) noexcept = delete;

private:
	void SetRasterizerState(ID3D11Device* pDevice) override;
};

