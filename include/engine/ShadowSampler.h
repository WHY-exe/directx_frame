#pragma once
#include "Bindable.h"
class ShadowSampler : public Bindable
{
public:
	ShadowSampler(Graphics& gfx);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};