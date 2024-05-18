#include "HorizontalBlurPass.h"
#include "Blender.h"
#include "ConstantBufferEx.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Sampler.h"
#include "Sink.h"
#include "Source.h"

namespace Rgph {
HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics &gfx,
                                       unsigned int fullWidth,
                                       unsigned int fullHeight)
    : FullscreenPass(std::move(name), gfx) {
  AddBind(PixelShader::Resolve(gfx, L"res\\cso\\BlurOutlinePS.cso"));
  AddBind(Blender::Resolve(gfx, false));
  AddBind(Sampler::Resolve(gfx, Sampler::Type::Point, true));

  AddBindSink<RenderTarget>("scratchIn");
  AddBindSink<CachingPixelConstantBuffer>("kernel");
  RegisterSink(DirectBindableSink<CachingPixelConstantBuffer>::Make("direction",
                                                                    direction));

  // the renderTarget is internally sourced and then exported as a Bindable
  renderTarget = std::make_shared<RenderTargetAsShaderTexture>(
      gfx, fullWidth / 2, fullHeight / 2, 0u);
  RegisterSource(
      DirectBindableSource<RenderTarget>::Make("scratchOut", renderTarget));
}

// this override is necessary because we cannot (yet) link input bindables
// directly into the container of bindables (mainly because vector growth
// buggers references)
void HorizontalBlurPass::Execute(Graphics &gfx) const noexcept(!IS_DEBUG) {
  auto buf = direction->GetBuffer();
  buf["isHorizontal"] = true;
  direction->SetBuffer(buf);

  direction->Bind(gfx);
  FullscreenPass::Execute(gfx);
}
} // namespace Rgph
