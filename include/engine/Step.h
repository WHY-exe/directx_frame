#pragma once
#include "Bindable.h"
#include "Probe.h"
#include "RenderGraph.h"
#include "RenderQueuePass.h"
#include <memory>
class Step {
public:
  Step(std::string szTargetName);
  Step(Step &&) = default;
  Step(const Step &src);
  Step &operator=(const Step &) = delete;
  Step &operator=(Step &&) = delete;
  void AddBind(std::shared_ptr<Bindable> bind) noexcept;
  void SetActiveState(bool active) noexcept;
  bool IsActive() const noexcept;
  void Submit(const class Drawable &d) const;
  void Bind(Graphics &gfx) const;
  void Link(Rgph::RenderGraph &rg);
  void Accept(Probe &probe) noexcept;

private:
  bool m_active_state = true;
  std::string m_szTargetName;
  Rgph::RenderQueuePass *m_pTargetPass = nullptr;
  std::vector<std::shared_ptr<Bindable>> m_Binds;
};
