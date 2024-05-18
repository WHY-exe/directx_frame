#include "Source.h"
#include "RenderGraphCompileException.h"
#include <algorithm>
#include <cctype>

namespace Rgph {
Source::Source(std::string nameIn) : m_name(std::move(nameIn)) {
  if (m_name.empty()) {
    throw RGC_EXCEPTION("Empty output name");
  }
  const bool nameCharsValid =
      std::all_of(m_name.begin(), m_name.end(),
                  [](char c) { return std::isalnum(c) || c == '_'; });
  if (!nameCharsValid || std::isdigit(m_name.front())) {
    throw RGC_EXCEPTION("Invalid output name: " + m_name);
  }
}

std::shared_ptr<Bindable> Source::YieldBindable() {
  throw RGC_EXCEPTION("Output cannot be accessed as bindable");
}

std::shared_ptr<BufferResource> Source::YieldBuffer() {
  throw RGC_EXCEPTION("Output cannot be accessed as buffer");
}

const std::string &Source::GetName() const noexcept { return m_name; }
} // namespace Rgph