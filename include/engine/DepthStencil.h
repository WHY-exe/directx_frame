#pragma once
#include "Bindable.h"
#include "Surface.h"
#include "BufferResource.h"
class DepthStencil : public Bindable, public BufferResource
{
	friend class RenderTarget;
public:
	enum class Usage
	{
		DepthStencil,
		DepthForShadow
	};
public:
	void BindAsBuffer(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noexcept(!IS_DEBUG) override;
	void BindAsBuffer(Graphics& gfx, class RenderTarget* renderTarget) noexcept(!IS_DEBUG);
	Surface ToSurface(Graphics& gfx, bool linearlize = true) const;
	void Clear(Graphics& gfx) noexcept(!IS_DEBUG) override;	
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetView() const noexcept;
protected:
	DepthStencil(Graphics& gfx, bool bindShaderResource, UINT width, UINT height, Usage usage);	
	DepthStencil(Graphics& gfx, bool bindShaderResource, Usage usage);
	DepthStencil(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTex, UINT face);
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_pDSV;
	UINT m_width;
	UINT m_height;
};

class DepthStencilAsShaderInput :public DepthStencil
{
public:
	DepthStencilAsShaderInput(Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil);
	DepthStencilAsShaderInput(Graphics& gfx, UINT slot, Usage usage = Usage::DepthStencil	);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void Resize(Graphics& gfx, UINT width, UINT height) noexcept(!IS_DEBUG) override;
private:
	UINT m_slot;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
};

class DepthStencilAsTraget : public DepthStencil
{
public:
	DepthStencilAsTraget(Graphics& gfx, UINT width, UINT height);
	DepthStencilAsTraget(Graphics& gfx);
	DepthStencilAsTraget(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTex, UINT face);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void Resize(Graphics& gfx, UINT width, UINT height) noexcept(!IS_DEBUG) override;
};