#pragma once
#include "Bindable.h"
#include "GfxThrowMacro.h"
class Viewport : public Bindable {
public:
  Viewport(Graphics &, UINT width, UINT height) {
    m_Viewport.Width = (float)width;
    m_Viewport.Height = (float)height;
    m_Viewport.MinDepth = 0;
    m_Viewport.MaxDepth = 1;
    m_Viewport.TopLeftX = 0;
    m_Viewport.TopLeftY = 0;
  }
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
    IMPORT_INFOMAN(gfx);
    GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetViewports(1u, &m_Viewport));
  }

private:
  D3D11_VIEWPORT m_Viewport{};
};