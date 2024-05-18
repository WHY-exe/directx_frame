#pragma once
#include "Bindable.h"
class ShadowRasterizer : public Bindable
{
public:
	ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp);
	void ResetShadowBiasParams(Graphics& gfx, int depthBias, float slopeBias, float clamp) noexcept(!IS_DEBUG);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	int	  GetDepthBias() const noexcept;
	float GetSlopeBias() const noexcept;
	float GetClamp()	 const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRSState;
	int   m_depthBias;
	float m_slopeBias;
	float m_clamp;
};