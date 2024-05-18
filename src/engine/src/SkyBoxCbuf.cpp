#include "SkyBoxCbuf.h"

SkyBoxCbuf::SkyBoxCbuf(Graphics &gfx, UINT slot)
    : m_vcbuf(std::make_unique<VertexConstantBuffer<Transform>>(gfx, slot)) {}

SkyBoxCbuf::Transform SkyBoxCbuf::GetTransform(Graphics &gfx) const noexcept {
  return {DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection())};
}

void SkyBoxCbuf::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  m_vcbuf->Update(gfx, GetTransform(gfx));
  m_vcbuf->Bind(gfx);
}

void SkyBoxCbuf::Update(Graphics &gfx) noexcept {
  Transform tf = {gfx.GetCamera() * gfx.GetProjection()};
  m_vcbuf->Update(gfx, tf);
}
