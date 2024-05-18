#include "FOVIndicator.h"
#include "BindableCommon.h"
#include "Channel.h"
FOVIndicator::FOVIndicator(Graphics &gfx, float width, float height,
                           float nearZ, float farZ) {
  SetVertices(gfx, width, height, nearZ, farZ);
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
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(7);
  }
  AddEssentialBind(IndexBuffer::Resolve(gfx, L"$fov", indices));
  AddEssentialBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));
  {
    Technique line("line_draw", Channel::main);
    {
      Step non_occluded("lambertian");
      auto pvs = VertexShader::Resolve(gfx, L"res\\cso\\Solid_VS.cso");
      Vertex::Layout layout;
      layout.Append(Vertex::Position3D);
      non_occluded.AddBind(InputLayout::Resolve(gfx, layout, *pvs));
      non_occluded.AddBind(std::move(pvs));
      non_occluded.AddBind(
          PixelShader::Resolve(gfx, L"res\\cso\\Solid_PS.cso"));
      struct PSColorConstant {
        DirectX::XMFLOAT3 color = {0.6f, 0.2f, 0.2f};
        float padding = 0.0f;
      } colorConst;
      non_occluded.AddBind(
          PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 3u));
      non_occluded.AddBind(std::make_shared<TransformCbuf>(gfx, *this));
      non_occluded.AddBind(Rasterizer::Resolve(gfx, false));
      line.AddStep(non_occluded);
    }
    {
      Step occluded("wireframe");
      auto pvs = VertexShader::Resolve(gfx, L"res\\cso\\Solid_VS.cso");
      Vertex::Layout layout;
      layout.Append(Vertex::Position3D);
      occluded.AddBind(InputLayout::Resolve(gfx, layout, *pvs));
      occluded.AddBind(std::move(pvs));
      occluded.AddBind(PixelShader::Resolve(gfx, L"res\\cso\\Solid_PS.cso"));
      struct PSColorConstant {
        DirectX::XMFLOAT3 color = {0.25f, 0.08f, 0.08f};
        float padding = 0.0f;
      } colorConst;
      occluded.AddBind(
          PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 3u));
      occluded.AddBind(std::make_shared<TransformCbuf>(gfx, *this));
      occluded.AddBind(Rasterizer::Resolve(gfx, false));
      line.AddStep(occluded);
    }
    AddTechnique(line);
  }
}

void FOVIndicator::SetVertices(Graphics &gfx, float width, float height,
                               float nearZ, float farZ) {
  Vertex::Layout layout;
  layout.Append(Vertex::Position3D);
  Vertex::DataBuffer vertices(layout);
  {
    const float zRatio = farZ / nearZ / 20.0f;
    const float nearX = width / 20.0f;
    const float nearY = height / 20.0f;
    const float farX = nearX * zRatio;
    const float farY = nearY * zRatio;
    vertices.EmplaceBack(DirectX::XMFLOAT3{-nearX, nearY, nearZ});
    vertices.EmplaceBack(DirectX::XMFLOAT3{nearX, nearY, nearZ});
    vertices.EmplaceBack(DirectX::XMFLOAT3{nearX, -nearY, nearZ});
    vertices.EmplaceBack(DirectX::XMFLOAT3{-nearX, -nearY, nearZ});
    vertices.EmplaceBack(DirectX::XMFLOAT3{-farX, farY, farZ});
    vertices.EmplaceBack(DirectX::XMFLOAT3{farX, farY, farZ});
    vertices.EmplaceBack(DirectX::XMFLOAT3{farX, -farY, farZ});
    vertices.EmplaceBack(DirectX::XMFLOAT3{-farX, -farY, farZ});
  }
  SetEssentialBind(0, std::make_shared<VertexBuffer>(gfx, L"$fov", vertices));
}

void FOVIndicator::SetPos(DirectX::XMFLOAT3 pos) noexcept { m_position = pos; }

void FOVIndicator::SetRot(DirectX::XMFLOAT3 rot) noexcept { m_rotation = rot; }

void FOVIndicator::SetScale(float scale) noexcept { m_scale = scale; }

DirectX::XMMATRIX FOVIndicator::GetTransformXM() const noexcept {
  return DirectX::XMMatrixScaling(m_scale, m_scale, m_scale) *
         DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y,
                                               m_rotation.z) *
         DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}
