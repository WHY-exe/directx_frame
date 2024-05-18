#include "Pass.h"
#include "RenderGraphCompileException.h"
#include "Sink.h"
#include "Source.h"
namespace Rgph {
Pass::Pass(const std::string &szPassName) noexcept : m_PassName(szPassName) {}

void Pass::Reset() noexcept(!IS_DEBUG) {}

const std::string &Pass::GetName() const noexcept { return m_PassName; }

const std::vector<std::unique_ptr<Sink>> &Pass::GetSinks() const {
  return m_sinks;
}
Source &Pass::GetSource(const std::string &name) const {
  for (auto &src : m_sources) {
    if (src->GetName() == name) {
      return *src;
    }
  }

  std::ostringstream oss;
  oss << "Output named [" << m_PassName << "] not found in pass: " << GetName();
  throw RGC_EXCEPTION(oss.str());
}
Sink &Pass::GetSink(const std::string &registeredName) const {
  for (auto &sink : m_sinks) {
    if (sink->GetRegisteredName() == registeredName) {
      return *sink;
    }
  }

  std::ostringstream oss;
  oss << "Output named [" << m_PassName << "] not found in pass: " << GetName();
  throw RGC_EXCEPTION(oss.str());
}
void Pass::SetSinkLinkage(const std::string &registeredName,
                          const std::string &target) {
  auto &sink = GetSink(registeredName);
  auto targetSplit = SplitString(target, ".");
  if (targetSplit.size() != 2u) {
    throw RGC_EXCEPTION("Input target has incorrect format");
  }
  sink.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
}
void Pass::Finalize() {
  for (auto &si : m_sinks) {
    si->PostLinkValidate();
  }
  for (auto &si : m_sources) {
    si->PostLinkValidate();
  }
}
Pass::~Pass() {}
void Pass::RegisterSink(std::unique_ptr<Sink> sink) {
  for (auto &si : m_sinks) {
    if (si->GetRegisteredName() == sink->GetRegisteredName())
      throw RGC_EXCEPTION("Registered input overlaps with existing: " +
                          sink->GetRegisteredName());
  }
  m_sinks.push_back(std::move(sink));
}
void Pass::RegisterSource(std::unique_ptr<Source> source) {
  for (auto &si : m_sources) {
    if (si->GetName() == source->GetName())
      throw RGC_EXCEPTION("Registered input overlaps with existing: " +
                          source->GetName());
  }
  m_sources.push_back(std::move(source));
}
} // namespace Rgph