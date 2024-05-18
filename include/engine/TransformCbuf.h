#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include <memory>
class TransformCbuf : public Bindable {
public:
  struct Transform {
    DirectX::XMMATRIX modelWorldView;
    DirectX::XMMATRIX modelCamView;
    DirectX::XMMATRIX modelProjView;
  };

public:
  TransformCbuf(Graphics &gfx, const Drawable &parent, UINT slot = 0);
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;

protected:
  Transform GetTransform(Graphics &gfx) const noexcept;
  void UpdateBindImpl(Graphics &gfx, const Transform &tf) const noexcept;

private:
  static std::unique_ptr<VertexConstantBuffer<Transform>> s_vcbuf;
  const Drawable &m_parent;
};
