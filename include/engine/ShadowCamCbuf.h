#pragma once
#include "Bindable.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "LightContainer.h"
class ShadowCamCbuf : public Bindable
{
public:
	struct Transform {
		DirectX::XMMATRIX Viewproj;
	};
public:
	ShadowCamCbuf(Graphics& gfx, UINT slot = 1u);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void Update(Graphics& gfx) noexcept;
	void SetCamera(std::shared_ptr<Camera> cam) noexcept;
private:
	std::unique_ptr<VertexConstantBuffer<Transform>> m_vcbuf;
	std::shared_ptr<Camera> m_pCamera;
};