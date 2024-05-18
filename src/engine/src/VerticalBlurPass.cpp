#include "VerticalBlurPass.h"
#include "Blender.h"
#include "ConstantBufferEx.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
namespace Rgph {
VerticalBlurPass::VerticalBlurPass(std::string name, Graphics &gfx)
    : FullscreenPass(std::move(name), gfx) {
  AddBind(PixelShader::Resolve(gfx, L"res\\cso\\BlurOutlinePS.cso"));
  AddBind(Blender::Resolve(gfx, true));
  AddBind(Stencil::Resolve(gfx, Stencil::Mod::Mask));
  AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear, true));

  AddBindSink<RenderTarget>("scratchIn");
  AddBindSink<CachingPixelConstantBuffer>("kernel");
  RegisterSink(DirectBindableSink<CachingPixelConstantBuffer>::Make("direction",
                                                                    direction));
  RegisterSink(
      DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
  RegisterSink(
      DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));

  RegisterSource(
      DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
  RegisterSource(
      DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
}

// see the note on HorizontalBlurPass::Execute
void VerticalBlurPass::Execute(Graphics &gfx) const noexcept(!IS_DEBUG) {
  auto buf = direction->GetBuffer();
  buf["isHorizontal"] = false;
  direction->SetBuffer(buf);

  direction->Bind(gfx);
  FullscreenPass::Execute(gfx);
}
} // namespace Rgph
