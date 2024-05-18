#pragma once
#include "Pass.h"

class BufferResource;

namespace Rgph {
class BufferClearPass : public Pass {
public:
  BufferClearPass(std::string name);
  void Execute(Graphics &gfx) const noexcept(!IS_DEBUG) override;

private:
  std::shared_ptr<BufferResource> buffer;
};
} // namespace Rgph