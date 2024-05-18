#include "ShadowCamCbuf.h"

ShadowCamCbuf::ShadowCamCbuf(Graphics &gfx, UINT slot)
    : m_vcbuf(std::make_unique<VertexConstantBuffer<Transform>>(gfx, slot)) {}

void ShadowCamCbuf::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  m_vcbuf->Bind(gfx);
}

void ShadowCamCbuf::Update(Graphics &gfx) noexcept {
  auto pos = m_pCamera->GetPos();
  const Transform tf{DirectX::XMMatrixTranspose(
      DirectX::XMMatrixTranslation(-pos.x, -pos.y, -pos.z))};
  m_vcbuf->Update(gfx, tf);
}

void ShadowCamCbuf::SetCamera(std::shared_ptr<Camera> cam) noexcept {
  m_pCamera = std::move(cam);
}
