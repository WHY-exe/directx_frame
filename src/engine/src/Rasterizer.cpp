#include "Rasterizer.h"
#include "BindableCodex.h"
#include "GfxThrowMacro.h"
#include <string>
#include <typeinfo>

Rasterizer::Rasterizer(Graphics &gfx, bool twoside) : m_TwoSide(twoside) {
  IMPORT_INFOMAN(gfx);

  D3D11_RASTERIZER_DESC raDesc = CD3D11_RASTERIZER_DESC{CD3D11_DEFAULT{}};
  raDesc.CullMode = twoside ? D3D11_CULL_NONE : D3D11_CULL_BACK;

  GFX_THROW_INFO(
      GetDevice(gfx)->CreateRasterizerState(&raDesc, &m_pRasterizerState));
}

void Rasterizer::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  GetContext(gfx)->RSSetState(m_pRasterizerState.Get());
}

std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics &gfx, bool twoside) {
  return CodeX::Resolve<Rasterizer>(gfx, twoside);
}

std::wstring Rasterizer::GetUID() noexcept { return GenUID(m_TwoSide); }

std::wstring Rasterizer::GenUID(bool twoside) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(typeid(Rasterizer).name() + "#"s + "TwoSide"s +
                          std::string(twoside ? "T"s : "F"s));
}
