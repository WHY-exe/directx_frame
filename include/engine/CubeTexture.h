#pragma once
#include "Bindable.h"
#include "Surface.h"

class DepthStencilAsTraget;
class CubeTexture : public Bindable {
public:
  CubeTexture(Graphics &gfx, const std::string &base_path,
              const std::vector<std::string> &paths, UINT slot = 0u);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pCubeTex;
  UINT m_slot;
};

class DepthCubeTexure : public Bindable {
public:
  DepthCubeTexure(Graphics &gfx, UINT size, UINT slot);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  std::shared_ptr<class DepthStencilAsTraget>
  GetDepthTarget(size_t index) noexcept;

private:
  UINT m_slot;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pCubeTex;
  std::vector<std::shared_ptr<class DepthStencilAsTraget>> m_depthTargets;
};