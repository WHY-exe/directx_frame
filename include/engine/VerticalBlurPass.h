#pragma once
#include "ConstantBufferEx.h"
#include "FullscreenPass.h"

class Graphics;
namespace Bind {
class PixelShader;
class RenderTarget;
} // namespace Bind

namespace Rgph {
class VerticalBlurPass : public FullscreenPass {
public:
  VerticalBlurPass(std::string name, Graphics &gfx);
  void Execute(Graphics &gfx) const noexcept(!IS_DEBUG) override;

private:
  std::shared_ptr<CachingPixelConstantBuffer> direction;
};
} // namespace Rgph