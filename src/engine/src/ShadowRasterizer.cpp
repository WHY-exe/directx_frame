#include "ShadowRasterizer.h"
#include "GfxThrowMacro.h"
ShadowRasterizer::ShadowRasterizer(Graphics &gfx, int depthBias,
                                   float slopeBias, float clamp) {
  ResetShadowBiasParams(gfx, depthBias, slopeBias, clamp);
}

void ShadowRasterizer::ResetShadowBiasParams(Graphics &gfx, int depthBias,
                                             float slopeBias,
                                             float clamp) noexcept(!IS_DEBUG) {
  m_depthBias = (depthBias);
  m_slopeBias = (slopeBias);
  m_clamp = (clamp);

  IMPORT_INFOMAN(gfx);
  D3D11_RASTERIZER_DESC raDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
  raDesc.DepthBiasClamp = m_clamp;
  raDesc.SlopeScaledDepthBias = m_slopeBias;
  raDesc.DepthBias = m_depthBias;
  GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&raDesc, &m_pRSState));
}

void ShadowRasterizer::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);
  GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(m_pRSState.Get()));
}

int ShadowRasterizer::GetDepthBias() const noexcept { return m_depthBias; }

float ShadowRasterizer::GetSlopeBias() const noexcept { return m_slopeBias; }

float ShadowRasterizer::GetClamp() const noexcept { return m_clamp; }
