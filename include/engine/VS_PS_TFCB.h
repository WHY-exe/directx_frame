#pragma once
#include "TransformCbuf.h"
class VS_PS_TFCB : public TransformCbuf {
public:
  VS_PS_TFCB(Graphics &gfx, const Drawable &parent, UINT slotV = 0u,
             UINT slotP = 3u);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;

protected:
  Transform GetTransform(Graphics &gfx) const noexcept;
  void UpdateBindImpl(Graphics &gfx, const Transform &tf) const noexcept;

private:
  static std::unique_ptr<PixelConstantBuffer<Transform>> s_pcbuf;
};
