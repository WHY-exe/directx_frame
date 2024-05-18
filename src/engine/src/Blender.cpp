#include "Blender.h"
#include "BindableCodex.h"
#include "GfxThrowMacro.h"
#include "StrManager.h"
#include <typeinfo>
Blender::Blender(Graphics &gfx, bool enBlend, std::optional<float> factor)
    : m_enBlend(enBlend) {
  IMPORT_INFOMAN(gfx);

  if (factor) {
    m_factors.emplace();
    m_factors->fill(*factor);
  }
  D3D11_BLEND_DESC blenddesc = {};
  blenddesc.AlphaToCoverageEnable = enBlend ? TRUE : FALSE;
  blenddesc.IndependentBlendEnable = enBlend ? TRUE : FALSE;
  // �Ƿ������
  blenddesc.RenderTarget[0].BlendEnable = enBlend ? TRUE : FALSE;
  blenddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
  blenddesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

  if (enBlend) {
    blenddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blenddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    if (factor) {
      blenddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
      blenddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
    }
    blenddesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blenddesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  }
  blenddesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blenddesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blenddesc.RenderTarget[0].RenderTargetWriteMask =
      D3D11_COLOR_WRITE_ENABLE_ALL;
  GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blenddesc, &m_pBlendState));
}

void Blender::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  const float *data = m_factors ? m_factors->data() : nullptr;
  GetContext(gfx)->OMSetBlendState(m_pBlendState.Get(), data, 0xFFFFFFFFu);
}

void Blender::SetFactor(float factor) noexcept {
  assert(m_factors);
  m_factors->fill(factor);
}

float Blender::GetFactor() const noexcept {
  assert(m_factors);
  return m_factors->front();
}

std::shared_ptr<Blender>
Blender::Resolve(Graphics &gfx, bool enBlend,
                 std::optional<float> factor) noexcept {
  return CodeX::Resolve<Blender>(gfx, enBlend, factor);
}

std::wstring Blender::GenUID(bool enBlend,
                             std::optional<float> factor) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(
      typeid(Blender).name() + "#"s + "Enblend"s +
      std::string(enBlend ? "T"s : "F"s) +
      std::string(factor ? "#f"s + std::to_string(*factor) : ""s));
}

std::wstring Blender::GetUID() const noexcept {
  return GenUID(m_enBlend,
                m_factors ? m_factors->front() : std::optional<float>{});
}
