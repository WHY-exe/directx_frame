#pragma once
#include "Pass.h"
#include "Sink.h"

class Bindable;

namespace Rgph {
class BindingPass : public Pass {
protected:
  BindingPass(std::string name,
              std::vector<std::shared_ptr<Bindable>> binds = {});
  void AddBind(std::shared_ptr<Bindable> bind) noexcept;
  void BindAll(Graphics &gfx) const noexcept(!IS_DEBUG);
  void Finalize() override;

protected:
  template <class T> void AddBindSink(std::string name) {
    const auto index = binds.size();
    binds.emplace_back();
    RegisterSink(std::make_unique<ContainerBindableSink<T>>(std::move(name),
                                                            binds, index));
  }
  std::shared_ptr<class RenderTarget> renderTarget;
  std::shared_ptr<class DepthStencil> depthStencil;

private:
  void BindBufferResources(Graphics &gfx) const noexcept(!IS_DEBUG);

private:
  std::vector<std::shared_ptr<Bindable>> binds;
};
} // namespace Rgph