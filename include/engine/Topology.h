#pragma once
#include "Bindable.h"
class Topology : public Bindable {
public:
  Topology(Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY type);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  static std::shared_ptr<Topology>
  Resolve(Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY type) noexcept(!IS_DEBUG);
  static std::wstring GenUID(D3D11_PRIMITIVE_TOPOLOGY type) noexcept;
  std::wstring GetUID() const noexcept override;

private:
  D3D11_PRIMITIVE_TOPOLOGY m_type;
};
