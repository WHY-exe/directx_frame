#include "BufferClearPass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Signal.h"
#include "Sink.h"
#include "Source.h"

namespace Rgph {
BufferClearPass::BufferClearPass(std::string name) : Pass(std::move(name)) {
  RegisterSink(DirectBufferSink<BufferResource>::Make("buffer", buffer));
  RegisterSource(DirectBufferSource<BufferResource>::Make("buffer", buffer));
}

void BufferClearPass::Execute(Graphics &gfx) const noexcept(!IS_DEBUG) {
  SIGNAL_CONDITIONAL(
      gfx.sizeSignalDS, dynamic_cast<DepthStencil *>(buffer.get()),
      buffer->Resize(gfx, gfx.GetWindowWidth(), gfx.GetWindowHeight()));
  buffer->Clear(gfx);
}
} // namespace Rgph