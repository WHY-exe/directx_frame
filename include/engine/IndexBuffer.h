#pragma once
#include "Bindable.h"
#include "GfxThrowMacro.h"
#include <memory>
class IndexBuffer : public Bindable {
public:
  IndexBuffer(Graphics &gfx, const std::vector<UINT> &indicies);
  IndexBuffer(Graphics &gfx, const std::wstring &tag,
              const std::vector<UINT> &indicies);
  static std::shared_ptr<IndexBuffer>
  Resolve(Graphics &gfx, const std::wstring &tag,
          const std::vector<UINT> &indicies) noexcept;
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  template <typename... Ignore>
  static std::wstring GenUID(std::wstring tag, Ignore &&...ingnore) noexcept {
    return GenUID_(tag);
  }
  std::wstring GetUID() const noexcept override;
  UINT GetSize() const;

private:
  static std::wstring GenUID_(std::wstring tag) noexcept;

private:
  std::wstring m_Tag;
  UINT m_uBufferSize;
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
};
