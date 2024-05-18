#include "Sink.h"
#include "Bindable.h"
#include "BufferResource.h"
#include "RenderGraphCompileException.h"
#include "Source.h"
#include "StrManager.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace Rgph {
Sink::Sink(std::string registeredNameIn)
    : m_registeredName(std::move(registeredNameIn)) {
  if (m_registeredName.empty()) {
    throw RGC_EXCEPTION("Empty output name");
  }
  const bool nameCharsValid =
      std::all_of(m_registeredName.begin(), m_registeredName.end(),
                  [](char c) { return std::isalnum(c) || c == '_'; });
  if (!nameCharsValid || std::isdigit(m_registeredName.front())) {
    throw RGC_EXCEPTION("Invalid output name: " + m_registeredName);
  }
}

const std::string &Sink::GetRegisteredName() const noexcept {
  return m_registeredName;
}

const std::string &Sink::GetPassName() const noexcept { return m_passName; }

const std::string &Sink::GetOutputName() const noexcept { return m_outputName; }

void Sink::SetTarget(std::string passName, std::string outputName) {
  {
    if (passName.empty()) {
      throw RGC_EXCEPTION("Empty output name");
    }
    const bool nameCharsValid =
        std::all_of(passName.begin(), passName.end(),
                    [](char c) { return std::isalnum(c) || c == '_'; });
    if (passName != "$" &&
        (!nameCharsValid || std::isdigit(passName.front()))) {
      throw RGC_EXCEPTION("Invalid output name: " + m_registeredName);
    }
    this->m_passName = passName;
  }
  {
    if (outputName.empty()) {
      throw RGC_EXCEPTION("Empty output name");
    }
    const bool nameCharsValid =
        std::all_of(outputName.begin(), outputName.end(),
                    [](char c) { return std::isalnum(c) || c == '_'; });
    if (!nameCharsValid || std::isdigit(outputName.front())) {
      throw RGC_EXCEPTION("Invalid output name: " + m_registeredName);
    }
    this->m_outputName = outputName;
  }
}
} // namespace Rgph
