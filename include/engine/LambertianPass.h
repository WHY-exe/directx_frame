#pragma once
#include "Camera.h"
#include "Job.h"
#include "RenderQueuePass.h"
#include "ShadowCamCbuf.h"
#include "ShadowSampler.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include <vector>
namespace Rgph {
class LambertianPass : public RenderQueuePass {
public:
  LambertianPass(Graphics &gfx, std::string name)
      : RenderQueuePass(std::move(name)),
        m_pShadowCbuf{std::make_shared<ShadowCamCbuf>(gfx)},
        m_pShadowSampler{std::make_shared<ShadowSampler>(gfx)} {
    AddBind(m_pShadowCbuf);
    AddBind(m_pShadowSampler);
    RegisterSink(
        DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
    RegisterSink(
        DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
    AddBindSink<Bindable>("ShadowMap");
    AddBindSink<Bindable>("shadowControl");
    RegisterSource(
        DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
    RegisterSource(
        DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
    AddBind(Stencil::Resolve(gfx, Stencil::Mod::Off));
  }
  void BindMainCamera(const Camera &cam) noexcept { m_pCamera = &cam; }
  void BindShadowCamera(std::shared_ptr<Camera> cam) noexcept {
    m_pShadowCbuf->SetCamera(std::move(cam));
  }
  void Execute(Graphics &gfx) const noexcept(!IS_DEBUG) override {
    assert(m_pCamera);
    m_pCamera->BindtoGFX(gfx);
    m_pShadowCbuf->Update(gfx);
    RenderQueuePass::Execute(gfx);
  }

private:
  const Camera *m_pCamera = nullptr;
  std::shared_ptr<ShadowCamCbuf> m_pShadowCbuf;
  std::shared_ptr<ShadowSampler> m_pShadowSampler;
};
} // namespace Rgph