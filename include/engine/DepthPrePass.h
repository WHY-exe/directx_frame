#pragma once
#include "RenderQueuePass.h"
#include "BindableCommon.h"
namespace Rgph
{
	class DepthPrePass : public RenderQueuePass
	{
	public:
		DepthPrePass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			depthStencil = std::make_shared<DepthStencilAsShaderInput>(gfx, gfx.GetWindowWidth(), gfx.GetWindowHeight());
			AddBind(PixelShader::Resolve(gfx, L"res\\cso\\DepthPrePass.cso"));

		}
	private:

	};
}