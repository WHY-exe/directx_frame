#include "Topology.h"
#include "BindableCodex.h"
Topology::Topology(Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY type)
    : m_type(type) {}

void Topology::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  GetContext(gfx)->IASetPrimitiveTopology(m_type);
}

std::shared_ptr<Topology>
Topology::Resolve(Graphics &gfx,
                  D3D11_PRIMITIVE_TOPOLOGY type) noexcept(!IS_DEBUG) {
  return CodeX::Resolve<Topology>(gfx, type);
}

std::wstring Topology::GenUID(D3D11_PRIMITIVE_TOPOLOGY type) noexcept {
  return ANSI_TO_UTF8_STR(typeid(Topology).name() + std::to_string(type));
}

std::wstring Topology::GetUID() const noexcept { return GenUID(m_type); }
