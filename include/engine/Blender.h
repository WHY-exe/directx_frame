#pragma once
#include "Bindable.h"
#include <array>
#include <memory>
#include <optional>
class Blender : public Bindable {
public:
  Blender(Graphics &gfx, bool enBlend, std::optional<float> factor = {});
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  void SetFactor(float factor) noexcept;
  float GetFactor() const noexcept;
  static std::shared_ptr<Blender>
  Resolve(Graphics &gfx, bool enBlend,
          std::optional<float> factor = {}) noexcept;
  std::wstring GetUID() const noexcept override;
  static std::wstring GenUID(bool enBlend,
                             std::optional<float> factor = {}) noexcept;

private:
  Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
  std::optional<std::array<float, 4>> m_factors;
  bool m_enBlend;
};
