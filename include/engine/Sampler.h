#pragma once
#include "Bindable.h"
#include <memory>
class Sampler : public Bindable
{
public:
	enum class Type
	{
		Anisotropic,
		Bilinear,
		Point,
	};
public:
	Sampler(Graphics& gfx, Type type = Type::Anisotropic, bool reflect = false,UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	static std::shared_ptr<Sampler> Resolve(Graphics& gfx, Type type = Type::Anisotropic, bool reflect = false, UINT slot = 0u) noexcept(!IS_DEBUG);
	static std::wstring GenUID(Type type, bool reflect) noexcept;
	std::wstring GetUID() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	Type m_type;
	UINT m_slot;
	bool m_reflect;
};

