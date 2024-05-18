#include "FullscreenPass.h"
#include "BindableCommon.h"

namespace Rgph {
namespace dx = DirectX;

FullscreenPass::FullscreenPass(const std::string name,
                               Graphics &gfx) noexcept(!IS_DEBUG)
    : BindingPass(std::move(name)) {
  // setup fullscreen geometry
  Vertex::Layout lay;
  lay.Append(Vertex::Position2D);
  Vertex::DataBuffer bufFull{lay};
  bufFull.EmplaceBack(dx::XMFLOAT2{-1, 1});
  bufFull.EmplaceBack(dx::XMFLOAT2{1, 1});
  bufFull.EmplaceBack(dx::XMFLOAT2{-1, -1});
  bufFull.EmplaceBack(dx::XMFLOAT2{1, -1});
  AddBind(VertexBuffer::Resolve(gfx, L"$Full", std::move(bufFull)));
  std::vector<UINT> indices = {0, 1, 2, 1, 3, 2};
  AddBind(IndexBuffer::Resolve(gfx, L"$Full", std::move(indices)));
  // setup other common fullscreen bindables
  auto vs = VertexShader::Resolve(gfx, L"res\\cso\\Full_VS.cso");
  AddBind(InputLayout::Resolve(gfx, lay, *vs));
  AddBind(std::move(vs));
  AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
  AddBind(Rasterizer::Resolve(gfx, false));
}

void FullscreenPass::Execute(Graphics &gfx) const noexcept(!IS_DEBUG) {
  BindAll(gfx);
  gfx.DrawIndexed(6u);
}
} // namespace Rgph
