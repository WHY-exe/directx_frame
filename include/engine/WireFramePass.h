#pragma once
#include "Job.h"
#include "RenderQueuePass.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include <vector>
namespace Rgph {
class WireFramePass : public RenderQueuePass {
public:
  WireFramePass(Graphics &gfx, std::string name)
      : RenderQueuePass(std::move(name)) {
    RegisterSink(
        DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
    RegisterSink(
        DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
    RegisterSource(
        DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
    RegisterSource(
        DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
    AddBind(Stencil::Resolve(gfx, Stencil::Mod::DepthReversed));
  }
};
} // namespace Rgph