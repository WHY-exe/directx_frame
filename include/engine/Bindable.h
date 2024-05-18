#pragma once
#include "Graphics.h"
#include "GraphicsResource.h"
#include "Probe.h"
#include "StrManager.h"
#include <string>
class Bindable : public GraphicsResource {
public:
  virtual void Bind(Graphics &gfx) noexcept(!IS_DEBUG) = 0;
  virtual ~Bindable() = default;
  virtual void InitializeParentReference(const class Drawable &) noexcept {}
  virtual void Accept(Probe &) noexcept {};
  virtual std::wstring GetUID() const noexcept { return L""; }
};
