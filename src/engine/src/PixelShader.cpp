#include "PixelShader.h"
#include "BindableCodex.h"
#include "GfxThrowMacro.h"
#include "StrManager.h"
#include <d3dcompiler.h>
PixelShader::PixelShader(Graphics &gfx, std::wstring szPath) : m_Path(szPath) {
  IMPORT_INFOMAN(gfx);
  Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
  GFX_THROW_INFO(D3DReadFileToBlob(szPath.c_str(), &pBlob));
  // create vertex shader
  GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(),
                                                   pBlob->GetBufferSize(),
                                                   nullptr, &m_pPixelShader));
}

void PixelShader::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  GetContext(gfx)->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);
}

std::shared_ptr<PixelShader>
PixelShader::Resolve(Graphics &gfx,
                     const std::wstring &path) noexcept(!IS_DEBUG) {
  return CodeX::Resolve<PixelShader>(gfx, path);
}

std::wstring PixelShader::GenUID(const std::wstring &path) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(typeid(PixelShader).name()) + L"#"s + path;
}

std::wstring PixelShader::GetUID() const noexcept { return GenUID(m_Path); }
