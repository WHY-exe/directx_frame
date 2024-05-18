#include "Step.h"
#include "Job.h"
Step::Step(std::string szTargetName)
    : m_szTargetName(std::move(szTargetName)) {}

Step::Step(const Step &src) : m_szTargetName(src.m_szTargetName) {
  m_Binds.reserve(src.m_Binds.size());
  for (auto i : src.m_Binds) {
    m_Binds.push_back(i);
  }
}

void Step::AddBind(std::shared_ptr<Bindable> bind) noexcept {
  m_Binds.push_back(std::move(bind));
}

void Step::SetActiveState(bool active) noexcept { m_active_state = active; }

bool Step::IsActive() const noexcept { return m_active_state; }

void Step::Submit(const Drawable &d) const {
  m_pTargetPass->Accept(Job(this, &d));
}

void Step::Bind(Graphics &gfx) const {
  for (auto &b : m_Binds) {
    b->Bind(gfx);
  }
}

void Step::Link(Rgph::RenderGraph &rg) {
  assert(!m_pTargetPass);
  m_pTargetPass = &rg.GetRenderQueue(m_szTargetName);
}

void Step::Accept(Probe &probe) noexcept {
  probe.SetStep(this);
  for (auto &i : m_Binds) {
    i->Accept(probe);
  }
}
