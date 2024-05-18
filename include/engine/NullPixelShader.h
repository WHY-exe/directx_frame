#pragma once
#include "Bindable.h"
#include "BindableCodex.h"
class NullPixelShader : public Bindable
{
public:
	NullPixelShader(Graphics& gfx);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	static std::shared_ptr<NullPixelShader> Resolve(Graphics& gfx) noexcept(!IS_DEBUG);
	static std::wstring GenUID() noexcept;
	std::wstring GetUID() const noexcept;
};