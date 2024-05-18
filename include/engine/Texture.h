#pragma once
#include "Bindable.h"
#include "Surface.h"
class Texture: public Bindable
{
public:
	Texture(Graphics& gfx, const std::wstring& path, unsigned int slot = 0);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::wstring& path, unsigned int slot = 0) noexcept(!IS_DEBUG);
	static std::wstring GenUID(const std::wstring& path, unsigned int slot = 0) noexcept;
	std::wstring GetUID() const noexcept override;
	bool HasAlpha() const noexcept;
private:
	unsigned int m_slot;
	std::wstring m_Path;
	bool m_hasAlpha;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexView;
};

