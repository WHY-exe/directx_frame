#include "Technique.h"
#include "Drawable.h"
Technique::Technique(const std::string &name, size_t channel) noexcept
    : m_Name(name), m_channel(channel) {}
void Technique::Submit(const Drawable &d, size_t channelFliter) const noexcept {
  if ((m_channel & channelFliter) != 0) {
    for (auto &s : m_Steps) {
      s.Submit(d);
    }
  }
}

void Technique::AddStep(Step &step) noexcept {
  m_Steps.push_back(std::move(step));
}

void Technique::SetStepActive(UINT step_index, bool active) noexcept {
  assert("index out of range" && step_index <= m_Steps.size());
  m_Steps[step_index].SetActiveState(active);
}

void Technique::SetActiveState(bool is_active) noexcept {
  m_activate = is_active;
  for (auto &i : m_Steps) {
    i.SetActiveState(m_activate);
  }
}

bool Technique::IsActive() const noexcept { return m_activate; }

void Technique::Accept(Probe &probe) noexcept {
  probe.SetTechnique(this);
  for (auto &i : m_Steps) {
    i.Accept(probe);
  }
}

void Technique::Link(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  for (auto &i : m_Steps) {
    i.Link(rg);
  }
}

const std::string &Technique::GetTechName() const noexcept { return m_Name; }
