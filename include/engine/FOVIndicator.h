#pragma once
#include "Drawable.h"
#include "Vertex.h"
#include <DirectXMath.h>
class FOVIndicator : public Drawable
{
public:
	FOVIndicator(class Graphics& gfx, float width, float height, float nearZ, float farZ);
	void SetVertices(Graphics& gfx, float width, float height, float nearZ, float farZ);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRot(DirectX::XMFLOAT3 rot) noexcept;
	void SetScale(float scale) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	float m_scale = 1.0f;
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };
};

