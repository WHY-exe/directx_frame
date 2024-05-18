#include "BlurOutlineRenderGraph.h"
#include "BlurOutlineDrawingPass.h"
#include "BufferClearPass.h"
#include "DynamicConstantBuffer.h"
#include "HorizontalBlurPass.h"
#include "LambertianPass.h"
#include "LightContainer.h"
#include "MathTool.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"
#include "RenderTarget.h"
#include "ShadowMappingPass.h"
#include "ShadowRasterizer.h"
#include "SkyBoxPass.h"
#include "Source.h"
#include "VerticalBlurPass.h"
#include "WireFramePass.h"
#include <imgui.h>
namespace Rgph {
BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics &gfx)
    : RenderGraph(gfx) {
  {
    auto pass = std::make_unique<BufferClearPass>("clearRT");
    pass->SetSinkLinkage("buffer", "$.backbuffer");
    AppendPass(std::move(pass));
  }
  {
    auto pass = std::make_unique<BufferClearPass>("clearDS");
    pass->SetSinkLinkage("buffer", "$.masterDepth");
    AppendPass(std::move(pass));
  }
  {
    shadowRasterizer =
        std::make_shared<ShadowRasterizer>(gfx, 100, 30.0f, 0.01f);
    AddGlobalSource(DirectBindableSource<ShadowRasterizer>::Make(
        "shadowRasterizer", shadowRasterizer));
  }
  {
    auto pass = std::make_unique<ShadowMappingPass>(gfx, "shadowMapping");
    pass->SetSinkLinkage("shadowRasterizer", "$.shadowRasterizer");
    AppendPass(std::move(pass));
  }
  {
    DCBuf::RawLayout l;
    l.Add<DCBuf::DataType::Integer>("pcfLevel");
    l.Add<DCBuf::DataType::Float>("depthBias");
    DCBuf::Buffer buffer(std::move(l));
    buffer["pcfLevel"] = 0;
    buffer["depthBias"] = 0.00005f;
    shadowControl =
        std::make_shared<CachingPixelConstantBuffer>(gfx, buffer, 3u);
    AddGlobalSource(DirectBindableSource<CachingPixelConstantBuffer>::Make(
        "shadowControl", shadowControl));
  }
  {
    auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
    pass->SetSinkLinkage("ShadowMap", "shadowMapping.ShadowMap");
    pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
    pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
    pass->SetSinkLinkage("shadowControl", "$.shadowControl");
    AppendPass(std::move(pass));
  }
  {
    auto pass = std::make_unique<WireFramePass>(gfx, "wireframe");
    pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
    pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
    AppendPass(std::move(pass));
  }
  {
    auto pass = std::make_unique<SkyBoxPass>(gfx, "skyBox");
    pass->SetSinkLinkage("renderTarget", "wireframe.renderTarget");
    pass->SetSinkLinkage("depthStencil", "wireframe.depthStencil");
    AppendPass(std::move(pass));
  }
  {
    auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
    pass->SetSinkLinkage("depthStencil", "skyBox.depthStencil");
    AppendPass(std::move(pass));
  }

  // setup blur constant buffers
  {
    {
      DCBuf::RawLayout l;
      l.Add<DCBuf::Integer>("nTaps");
      l.Add<DCBuf::Array>("coefficients");
      l["coefficients"].Set<DCBuf::Float>(maxRadius * 2 + 1);
      DCBuf::Buffer buf{std::move(l)};
      blurKernel = std::make_shared<CachingPixelConstantBuffer>(gfx, buf, 0);
      SetKernelGauss(radius, sigma);
      AddGlobalSource(DirectBindableSource<CachingPixelConstantBuffer>::Make(
          "blurKernel", blurKernel));
    }
    {
      DCBuf::RawLayout l;
      l.Add<DCBuf::Bool>("isHorizontal");
      DCBuf::Buffer buf{std::move(l)};
      blurDirection = std::make_shared<CachingPixelConstantBuffer>(gfx, buf, 1);
      AddGlobalSource(DirectBindableSource<CachingPixelConstantBuffer>::Make(
          "blurDirection", blurDirection));
    }
  }

  {
    auto pass = std::make_unique<BlurOutlineDrawingPass>(
        gfx, "outlineDraw", gfx.GetWindowWidth(), gfx.GetWindowHeight());
    AppendPass(std::move(pass));
  }
  {
    auto pass = std::make_unique<HorizontalBlurPass>(
        "horizontal", gfx, gfx.GetWindowWidth(), gfx.GetWindowHeight());
    pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOut");
    pass->SetSinkLinkage("kernel", "$.blurKernel");
    pass->SetSinkLinkage("direction", "$.blurDirection");
    AppendPass(std::move(pass));
  }
  {
    auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
    pass->SetSinkLinkage("renderTarget", "skyBox.renderTarget");
    pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
    pass->SetSinkLinkage("scratchIn", "horizontal.scratchOut");
    pass->SetSinkLinkage("kernel", "$.blurKernel");
    pass->SetSinkLinkage("direction", "$.blurDirection");
    AppendPass(std::move(pass));
  }

  SetSinkTarget("backbuffer", "vertical.renderTarget");

  Finalize();
}

void BlurOutlineRenderGraph::SetKernelGauss(int radius,
                                            float sigma) noexcept(!IS_DEBUG) {
  assert(radius <= maxRadius);
  auto k = blurKernel->GetBuffer();
  const int nTaps = radius * 2 + 1;
  k["nTaps"] = nTaps;
  float sum = 0.0f;
  for (int i = 0; i < nTaps; i++) {
    const auto x = float(i - radius);
    const auto g = math_tool::gauss(x, sigma);
    sum += g;
    k["coefficients"][i] = g;
  }
  for (int i = 0; i < nTaps; i++) {
    k["coefficients"][i] = (float)k["coefficients"][i] / sum;
  }
  blurKernel->SetBuffer(k);
}
void BlurOutlineRenderGraph::SetKernelBox(int radius) noexcept(!IS_DEBUG) {
  assert(radius <= maxRadius);
  auto k = blurKernel->GetBuffer();
  const int nTaps = radius * 2 + 1;
  k["nTaps"] = nTaps;
  const float c = 1.0f / nTaps;
  for (int i = 0; i < nTaps; i++) {
    k["coefficients"][i] = c;
  }
  blurKernel->SetBuffer(k);
}

void BlurOutlineRenderGraph::RenderBlurWidgets(Graphics &gfx) {
  if (ImGui::Begin("Kernel")) {
    bool filterChanged = false;
    {
      const char *items[] = {"Gauss", "Box"};
      static const char *curItem = items[0];
      if (ImGui::BeginCombo("Filter Type", curItem)) {
        for (int n = 0; n < std::size(items); n++) {
          const bool isSelected = (curItem == items[n]);
          if (ImGui::Selectable(items[n], isSelected)) {
            filterChanged = true;
            curItem = items[n];
            if (curItem == items[0]) {
              m_KernelType = KernelType::Gauss;
            } else if (curItem == items[1]) {
              m_KernelType = KernelType::Box;
            }
          }
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
    }

    bool radChange = ImGui::SliderInt("Radius", &radius, 0, maxRadius);
    bool sigChange = false;
    if (m_KernelType == KernelType::Gauss) {
      sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);
    }
    if (radChange || sigChange || filterChanged) {
      if (m_KernelType == KernelType::Gauss) {
        SetKernelGauss(radius, sigma);
      } else if (m_KernelType == KernelType::Box) {
        SetKernelBox(radius);
      }
    }
  }
  ImGui::End();
}
void BlurOutlineRenderGraph::RenderShadowWigets(Graphics &gfx) {
  if (ImGui::Begin("Shadows")) {
    float clamp = shadowRasterizer->GetClamp();
    int depthBias = shadowRasterizer->GetDepthBias();
    float slopBias = shadowRasterizer->GetSlopeBias();

    auto ctrl = shadowControl->GetBuffer();
    bool pcfChange = ImGui::SliderInt("PCF Level", &ctrl["pcfLevel"], 0, 4);
    bool biasChange =
        ImGui::SliderFloat("Depth Bias", &ctrl["depthBias"], 0.0f, 0.1f, "%.6f",
                           ImGuiSliderFlags_Logarithmic);

    bool clampChange = ImGui::SliderFloat("Clamp", &clamp, 0.0001f, 0.5f,
                                          "%.4f", ImGuiSliderFlags_Logarithmic);
    bool depthBiasChange = ImGui::SliderInt(
        "PreBias", &depthBias, 0, 10000, "%.0f", ImGuiSliderFlags_Logarithmic);
    bool slopBiasChange =
        ImGui::SliderFloat("Slop Bias", &slopBias, 0, 300.0f, "%.4f");

    if (pcfChange || biasChange) {
      shadowControl->SetBuffer(ctrl);
    }
    if (clampChange || depthBiasChange || slopBiasChange) {
      shadowRasterizer->ResetShadowBiasParams(gfx, depthBias, slopBias, clamp);
    }
  }
  ImGui::End();
}
void BlurOutlineRenderGraph::DumpShadowMap(Graphics &gfx,
                                           const std::string &path) {
  // dynamic_cast<ShadowMappingPass&>(FindPassByName("shadowMapping")).DumpShadowMap(gfx,
  // path);
}
void BlurOutlineRenderGraph::BindMainCamera(const Camera &cam) noexcept(
    !IS_DEBUG) {
  dynamic_cast<LambertianPass &>(FindPassByName("lambertian"))
      .BindMainCamera(cam);
  dynamic_cast<SkyBoxPass &>(FindPassByName("skyBox")).BindMainCamera(cam);
}
void BlurOutlineRenderGraph::BindShadowCamera(
    const LightContainer &lights) noexcept(!IS_DEBUG) {
  dynamic_cast<ShadowMappingPass &>(FindPassByName("shadowMapping"))
      .BindCamera(std::move(lights.GetContainer()[0]->ShareCamera()));
  dynamic_cast<LambertianPass &>(FindPassByName("lambertian"))
      .BindShadowCamera(std::move(lights.GetContainer()[0]->ShareCamera()));
}
void BlurOutlineRenderGraph::BindAddedShadowCamera(
    std::shared_ptr<Camera> cam) noexcept(!IS_DEBUG) {
  dynamic_cast<LambertianPass &>(FindPassByName("lambertian"))
      .BindShadowCamera(cam);
}
} // namespace Rgph
