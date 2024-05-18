#include "Drawable.h"
#include "ConstantBufferEx.h"
#include "TransformCbuf.h"

void Drawable::AddTechnique(Technique &tech) noexcept {
  m_Techs.push_back(std::move(tech));
}

void Drawable::SetEssentialBind(size_t index,
                                std::shared_ptr<Bindable> bind) noexcept {
  if (m_essential_binds.empty()) {
    AddEssentialBind(std::move(bind));
  } else {
    assert(index < m_essential_binds.size());
    m_essential_binds[index] = bind;
  }
}

void Drawable::AddEssentialBind(std::shared_ptr<Bindable> bind) noexcept {
  if (typeid(*bind) == typeid(IndexBuffer)) {
    assert("Attempting to add index buffer a second time" &&
           m_pIndexBuffer == nullptr);
    m_pIndexBuffer = &static_cast<IndexBuffer &>(*bind);
  }
  m_essential_binds.push_back(std::move(bind));
}

void Drawable::Bind(Graphics &gfx) const noexcept {
  for (auto &b : m_essential_binds) {
    b->Bind(gfx);
  }
}

void Drawable::Accept(Probe &probe) noexcept {
  for (auto &i : m_Techs) {
    i.Accept(probe);
  }
}

void Drawable::LinkTechniques(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  for (auto &i : m_Techs) {
    i.Link(rg);
  }
}

void Drawable::Submit(size_t channel) const noexcept {
  for (auto &tech : m_Techs) {
    tech.Submit(*this, channel);
  }
}

UINT Drawable::GetIndexSize() const noexcept {
  return m_pIndexBuffer->GetSize();
}
