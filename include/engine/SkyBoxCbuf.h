#pragma once
#include "Bindable.h"
#include "ConstantBuffer.h"
class SkyBoxCbuf : public Bindable
{
public:
	struct Transform {
		DirectX::XMMATRIX Viewproj;
	};
public:
	SkyBoxCbuf(Graphics& gfx, UINT slot = 0u);
	Transform GetTransform(Graphics& gfx) const noexcept;
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void Update(Graphics& gfx) noexcept;
private:
	std::unique_ptr<VertexConstantBuffer<Transform>> m_vcbuf;
};

