#pragma once
#include "Bindable.h"
#include "BufferResource.h"
#include <array>
class DeferredBuffer : public Bindable, public BufferResource {
public:
  void BindAsBuffer(Graphics &gfx) noexcept(!IS_DEBUG) override;
  void BindAsBuffer(Graphics &,
                    BufferResource *) noexcept(!IS_DEBUG) override{};
  void Clear(Graphics &gfx, std::array<float, 4> color) noexcept(!IS_DEBUG);
  void Clear(Graphics &gfx) noexcept(!IS_DEBUG) override;

protected:
  static constexpr size_t buffer_num = 4;
  DeferredBuffer(Graphics &gfx, UINT width, UINT height);
  DeferredBuffer(Graphics &gfx);
  UINT m_width, m_height;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTVs[buffer_num];
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;
};

class DeferredBufferAsShaderInput : public DeferredBuffer {
public:
  DeferredBufferAsShaderInput(Graphics &gfx, UINT width, UINT height,
                              UINT slot = 0);
  DeferredBufferAsShaderInput(Graphics &gfx, UINT slot = 0);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  void Resize(Graphics &gfx, UINT width, UINT height) override;

private:
  UINT m_slot;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_PSRV[buffer_num];
};

class DeferredBufferAsTarget : public DeferredBuffer {
public:
  DeferredBufferAsTarget(Graphics &gfx, UINT width, UINT height);
  DeferredBufferAsTarget(Graphics &gfx);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  void Resize(Graphics &gfx, UINT width, UINT height) override;
};