#include "LightIndicator.h"
#include "BindableCommon.h"
#include "Channel.h"
#include "Sphere.h"
LightIndicator::LightIndicator(Graphics &gfx) {
  const auto geometryTag = L"$light";
  auto sphere = Sphere::Make();

  AddEssentialBind(VertexBuffer::Resolve(gfx, geometryTag, sphere.vertices));
  AddEssentialBind(IndexBuffer::Resolve(gfx, geometryTag, sphere.indices));
  AddEssentialBind(
      Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

  {
    Technique line("normal_draw", Channel::main);
    Step only("lambertian");

    auto pvs = VertexShader::Resolve(gfx, L"res\\cso\\Solid_VS.cso");
    only.AddBind(InputLayout::Resolve(gfx, sphere.vertices.GetLayout(), *pvs));
    only.AddBind(std::move(pvs));

    only.AddBind(PixelShader::Resolve(gfx, L"res\\cso\\lightBall_PS.cso"));

    only.AddBind(std::make_shared<TransformCbuf>(gfx, *this));

    only.AddBind(Rasterizer::Resolve(gfx, false));

    line.AddStep(only);
    AddTechnique(line);
  }
}

void LightIndicator::SetPos(DirectX::XMFLOAT3 pos) noexcept {
  m_position = pos;
}

void LightIndicator::SetRot(DirectX::XMFLOAT3 rot) noexcept {
  m_rotation = rot;
}

void LightIndicator::SetScale(float scale) noexcept { m_scale = scale; }

DirectX::XMMATRIX LightIndicator::GetTransformXM() const noexcept {
  return DirectX::XMMatrixScaling(m_scale, m_scale, m_scale) *
         DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y,
                                               m_rotation.z) *
         DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}
