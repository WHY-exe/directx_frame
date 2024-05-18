#include "CameraIndicator.h"
#include "BindableCommon.h"
#include "Channel.h"
#include "Probe.h"

CameraIndicator::CameraIndicator(Graphics &gfx) {
  const std::wstring geometry_tag = L"$cam";
  Vertex::Layout layout;
  layout.Append(Vertex::Position3D);
  Vertex::DataBuffer vertices(layout);
  {
    const float x = 4.0f / 3.0f * 0.75f;
    const float y = 1.0f * 0.75f;
    const float z = -2.0f;
    const float thalf = x * 0.5f;
    const float tspace = y * 0.2f;
    vertices.EmplaceBack(DirectX::XMFLOAT3{-x, y, 0.0f});
    vertices.EmplaceBack(DirectX::XMFLOAT3{x, y, 0.0f});
    vertices.EmplaceBack(DirectX::XMFLOAT3{x, -y, 0.0f});
    vertices.EmplaceBack(DirectX::XMFLOAT3{-x, -y, 0.0f});
    vertices.EmplaceBack(DirectX::XMFLOAT3{0.0f, 0.0f, z});
    vertices.EmplaceBack(DirectX::XMFLOAT3{-thalf, y + tspace, 0.0f});
    vertices.EmplaceBack(DirectX::XMFLOAT3{thalf, y + tspace, 0.0f});
    vertices.EmplaceBack(DirectX::XMFLOAT3{0.0f, y + tspace + thalf, 0.0f});
  }
  std::vector<unsigned int> indices;
  {
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(3);
    indices.push_back(0);
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(4);
    indices.push_back(2);
    indices.push_back(4);
    indices.push_back(3);
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(7);
    indices.push_back(5);
  }
  AddEssentialBind(VertexBuffer::Resolve(gfx, geometry_tag, vertices));
  AddEssentialBind(IndexBuffer::Resolve(gfx, geometry_tag, indices));
  AddEssentialBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));

  {
    Technique line("line_draw", Channel::main);
    Step only("lambertian");
    auto pvs = VertexShader::Resolve(gfx, L"res\\cso\\Solid_VS.cso");
    only.AddBind(InputLayout::Resolve(gfx, vertices.GetLayout(), *pvs));
    only.AddBind(std::move(pvs));
    struct PSColorConstant {
      DirectX::XMFLOAT3 color = {0.2f, 0.2f, 0.6f};
      float padding = 0.0f;
    } colorConst;
    only.AddBind(
        PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 3u));
    only.AddBind(PixelShader::Resolve(gfx, L"res\\cso\\Solid_PS.cso"));
    only.AddBind(std::make_shared<TransformCbuf>(gfx, *this));
    only.AddBind(Rasterizer::Resolve(gfx, false));
    line.AddStep(only);
    AddTechnique(line);
  }
}
void CameraIndicator::SetPos(DirectX::XMFLOAT3 pos) noexcept {
  m_position = pos;
}

void CameraIndicator::SetRot(DirectX::XMFLOAT3 rot) noexcept {
  m_rotation = rot;
}

void CameraIndicator::SetScale(float scale) noexcept { m_scale = scale; }

DirectX::XMMATRIX CameraIndicator::GetTransformXM() const noexcept {
  return DirectX::XMMatrixScaling(m_scale, m_scale, m_scale) *
         DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y,
                                               m_rotation.z) *
         DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}