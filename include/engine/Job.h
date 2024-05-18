#pragma once
#include "Graphics.h"

class Job {
public:
  Job(const class Step *step, const class Drawable *drawable);
  void Execute(Graphics &gfx) const noexcept(!IS_DEBUG);

private:
  const class Step *m_pStep;
  const class Drawable *m_pDrawable;
};
