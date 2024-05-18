#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include <memory>
class InputLayout : public Bindable {
public:
  InputLayout(Graphics &gfx, const Vertex::Layout &layout,
              const class VertexShader &vs);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;
  static std::shared_ptr<InputLayout>
  Resolve(Graphics &gfx, const Vertex::Layout &layout,
          const class VertexShader &vs) noexcept(!IS_DEBUG);
  static std::wstring GenUID(const Vertex::Layout &layout,
                             const class VertexShader &vs) noexcept;
  std::wstring GetUID() const noexcept override;

private:
  const class VertexShader &m_vs;
  const Vertex::Layout &m_layout;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
};
