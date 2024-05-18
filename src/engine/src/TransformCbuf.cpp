#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics &gfx, const Drawable &parent, UINT slot)
    : m_parent(parent) {
  if (!s_vcbuf) {
    s_vcbuf = std::make_unique<VertexConstantBuffer<Transform>>(gfx, slot);
  }
}
void TransformCbuf::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  UpdateBindImpl(gfx, GetTransform(gfx));
  s_vcbuf->Bind(gfx);
}
TransformCbuf::Transform
TransformCbuf::GetTransform(Graphics &gfx) const noexcept {
  return {
      DirectX::XMMatrixTranspose(m_parent.GetTransformXM()),
      DirectX::XMMatrixTranspose(m_parent.GetTransformXM() * gfx.GetCamera()),
      DirectX::XMMatrixTranspose(m_parent.GetTransformXM() * gfx.GetCamera() *
                                 gfx.GetProjection()),
  };
}
void TransformCbuf::UpdateBindImpl(Graphics &gfx,
                                   const Transform &tf) const noexcept {
  s_vcbuf->Update(gfx, tf);
}
std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transform>>
    TransformCbuf::s_vcbuf;