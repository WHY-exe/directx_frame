#pragma once
#include "Bindable.h"
#include <memory>
class VertexShader : public Bindable {
public:
  VertexShader(Graphics &gfx, const std::wstring szPath);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  ID3DBlob *GetByteCode() const noexcept;
  static std::shared_ptr<VertexShader>
  Resolve(Graphics &gfx, const std::wstring &path) noexcept(!IS_DEBUG);
  static std::wstring GenUID(const std::wstring &path) noexcept;
  std::wstring GetUID() const noexcept override;

protected:
  std::wstring m_szPath;
  Microsoft::WRL::ComPtr<ID3DBlob> m_pByteCodeBlob;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
};
