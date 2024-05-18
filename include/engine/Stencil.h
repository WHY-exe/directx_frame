#pragma once
#include "Bindable.h"
#include <memory>
#include <string>
class Stencil : public Bindable {
public:
  enum class Mod {
    Off,
    DepthOff,
    DepthReversed,
    DepthFirst,
    Write,
    Mask,
    MaskAndWrite
  };

public:
  Stencil(Graphics &gfx, Mod mod);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  static std::shared_ptr<Stencil> Resolve(Graphics &gfx,
                                          Mod mode) noexcept(!IS_DEBUG);
  static std::wstring GenUID(Mod mode) noexcept;
  std::wstring GetUID() const noexcept override;

private:
  Mod m_mode;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pStencilState;
};