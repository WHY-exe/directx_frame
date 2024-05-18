#pragma once
#include "Bindable.h"
#include "Surface.h"
#include "BufferResource.h"
#include <array>
class RenderTarget : public Bindable, public BufferResource
{
public:
	RenderTarget(Graphics& gfx, UINT width, UINT height);
	RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
	void BindAsBuffer(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noexcept(!IS_DEBUG) override;
	void BindAsBuffer(Graphics& gfx, class DepthStencil* ds) noexcept(!IS_DEBUG);
	void Remake(Graphics& gfx, ID3D11Texture2D* pTexture);
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	void CleanUp() noexcept(!IS_DEBUG);
	void Clear(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void Clear(Graphics& gfx, std::array<float, 4> color) noexcept(!IS_DEBUG);
protected:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
private:
	UINT m_uWidth, m_uHeight;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_pTarget;
};

class RenderTargetAsShaderTexture : public RenderTarget
{
public:
	RenderTargetAsShaderTexture(Graphics& gfx, UINT width, UINT height, UINT slot);
	RenderTargetAsShaderTexture(Graphics& gfx, ID3D11Texture2D* pTexture, UINT slot);
	Surface ToSurface(Graphics& gfx) noexcept(!IS_DEBUG);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	void Resize(Graphics& gfx, UINT width, UINT height) override;
private:
	UINT m_slot;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_pPSTextureView;
};

class RenderTargetAsOutputTarget : public RenderTarget
{
public:
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	RenderTargetAsOutputTarget(Graphics& gfx, UINT width, UINT height);
	RenderTargetAsOutputTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
	void Resize(Graphics& gfx, UINT width, UINT height) override;
};