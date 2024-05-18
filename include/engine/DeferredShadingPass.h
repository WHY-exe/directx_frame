#pragma once
#include "RenderQueuePass.h"
#include "DeferredBuffer.h"
#include "BindableCommon.h"
namespace Rgph
{
	class DeferredShadingPass : public RenderQueuePass
	{
	public:
		DeferredShadingPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			m_dfbuf = std::make_shared<DeferredBufferAsShaderInput>(gfx);

		}
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override
		{
			m_dfbuf->Clear(gfx);
			m_dfbuf->BindAsBuffer(gfx);
			RenderQueuePass::Execute(gfx);
		}
	private:
		std::shared_ptr<DeferredBufferAsShaderInput> m_dfbuf;
	};
}