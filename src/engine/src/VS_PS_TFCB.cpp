#include "VS_PS_TFCB.h"

VS_PS_TFCB::VS_PS_TFCB(Graphics &gfx, const Drawable &parent, UINT slotV,
                       UINT slotP)
    : TransformCbuf(gfx, parent, slotV) {
  if (!s_pcbuf) {
    s_pcbuf = std::make_unique<PixelConstantBuffer<Transform>>(gfx, slotP);
  }
}

void VS_PS_TFCB::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  TransformCbuf::Bind(gfx);
  UpdateBindImpl(gfx, GetTransform(gfx));
  s_pcbuf->Bind(gfx);
}

TransformCbuf::Transform
VS_PS_TFCB::GetTransform(Graphics &gfx) const noexcept {
  return TransformCbuf::GetTransform(gfx);
}

void VS_PS_TFCB::UpdateBindImpl(Graphics &gfx,
                                const Transform &tf) const noexcept {
  s_pcbuf->Update(gfx, tf);
  s_pcbuf->Bind(gfx);
}
std::unique_ptr<PixelConstantBuffer<VS_PS_TFCB::Transform>> VS_PS_TFCB::s_pcbuf;