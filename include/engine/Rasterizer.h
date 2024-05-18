#pragma once
#include "Bindable.h"
#include <memory>
class Rasterizer:public Bindable
{
public:
	Rasterizer(Graphics& gfx, bool twoside);
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoside);
	std::wstring GetUID() noexcept;	
	static std::wstring GenUID(bool twoside) noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	bool m_TwoSide;
};

