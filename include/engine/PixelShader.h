#pragma once
#include "Bindable.h"
#include <memory>
class PixelShader : public Bindable {
public:
  PixelShader(Graphics &gfx, std::wstring szPath);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  static std::shared_ptr<PixelShader>
  Resolve(Graphics &gfx, const std::wstring &path) noexcept(!IS_DEBUG);
  static std::wstring GenUID(const std::wstring &path) noexcept;
  std::wstring GetUID() const noexcept override;

private:
  std::wstring m_Path;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
};
