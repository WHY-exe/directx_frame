#include "InputLayout.h"
#include "BindableCodex.h"
#include "GfxThrowMacro.h"
#include "StrManager.h"
#include "VertexShader.h"
#include <typeinfo>
InputLayout::InputLayout(Graphics &gfx, const Vertex::Layout &layout,
                         const VertexShader &vs)
    : m_layout(layout), m_vs(vs) {
  auto D3DLayout = m_layout.GetD3DLayout();
  auto byte_code = vs.GetByteCode();
  IMPORT_INFOMAN(gfx);
  GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
      D3DLayout.data(), (UINT)D3DLayout.size(), byte_code->GetBufferPointer(),
      byte_code->GetBufferSize(), &m_pInputLayout));
}

void InputLayout::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  GetContext(gfx)->IASetInputLayout(m_pInputLayout.Get());
}

std::shared_ptr<InputLayout>
InputLayout::Resolve(Graphics &gfx, const Vertex::Layout &layout,
                     const VertexShader &vs) noexcept(!IS_DEBUG) {
  return CodeX::Resolve<InputLayout>(gfx, layout, vs);
}

std::wstring InputLayout::GetUID() const noexcept {
  return GenUID(m_layout, m_vs);
}

std::wstring InputLayout::GenUID(const Vertex::Layout &layout,
                                 const VertexShader &vs) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(typeid(InputLayout).name()) + L"#" +
         ANSI_TO_UTF8_STR(layout.GetCode()) + vs.GetUID();
}
