#include "VertexShader.h"
#include "BindableCodex.h"
#include "GfxThrowMacro.h"
#include "StrManager.h"
#include <d3dcompiler.h>
#include <memory>
#include <typeinfo>
VertexShader::VertexShader(Graphics &gfx, const std::wstring szPath)
    : m_szPath(szPath) {
  IMPORT_INFOMAN(gfx);
  // read the compiled vertex shader to the memory
  GFX_THROW_INFO(D3DReadFileToBlob(szPath.c_str(), &m_pByteCodeBlob));
  // create vertex shader
  GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
      m_pByteCodeBlob->GetBufferPointer(), m_pByteCodeBlob->GetBufferSize(),
      nullptr, &m_pVertexShader));
}

void VertexShader::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  // bind vertex shader to the pipeline
  GetContext(gfx)->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob *VertexShader::GetByteCode() const noexcept {
  return m_pByteCodeBlob.Get();
}

std::shared_ptr<VertexShader>
VertexShader::Resolve(Graphics &gfx,
                      const std::wstring &path) noexcept(!IS_DEBUG) {
  return CodeX::Resolve<VertexShader>(gfx, path);
}

std::wstring VertexShader::GenUID(const std::wstring &path) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(typeid(VertexShader).name()) + L"#"s + path;
}

std::wstring VertexShader::GetUID() const noexcept { return GenUID(m_szPath); }
