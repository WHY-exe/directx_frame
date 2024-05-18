#include "RenderGraph.h"
#include "BindableCommon.h"
#include "DepthStencil.h"
#include "Pass.h"
#include "RenderGraphCompileException.h"
#include "RenderQueuePass.h"
#include "RenderTarget.h"
#include "Sink.h"
#include "Source.h"
#include <sstream>

namespace Rgph {
RenderGraph::RenderGraph(Graphics &gfx)
    : m_backBufferTarget(gfx.GetTarget()),
      m_masterDepth(std::make_shared<DepthStencilAsTraget>(gfx)) {
  // setup global sinks and sources
  AddGlobalSource(
      DirectBufferSource<RenderTarget>::Make("backbuffer", m_backBufferTarget));
  AddGlobalSource(
      DirectBufferSource<DepthStencil>::Make("masterDepth", m_masterDepth));
  AddGlobalSink(
      DirectBufferSink<RenderTarget>::Make("backbuffer", m_backBufferTarget));
}

RenderGraph::~RenderGraph() {}

void RenderGraph::SetSinkTarget(const std::string &sinkName,
                                const std::string &target) {
  const auto finder = [&sinkName](const std::unique_ptr<Sink> &p) {
    return p->GetRegisteredName() == sinkName;
  };
  const auto i =
      std::find_if(m_globalSinks.begin(), m_globalSinks.end(), finder);
  if (i == m_globalSinks.end()) {
    throw RGC_EXCEPTION("Global sink does not exist: " + sinkName);
  }
  auto targetSplit = SplitString(target, ".");
  if (targetSplit.size() != 2u) {
    throw RGC_EXCEPTION("Input target has incorrect format");
  }
  (*i)->SetTarget(targetSplit[0], targetSplit[1]);
}

void RenderGraph::AddGlobalSource(std::unique_ptr<Source> out) {
  m_globalSources.push_back(std::move(out));
}

void RenderGraph::AddGlobalSink(std::unique_ptr<Sink> in) {
  m_globalSinks.push_back(std::move(in));
}

void RenderGraph::Execute(Graphics &gfx) noexcept(!IS_DEBUG) {
  assert(finalized);
  for (auto &p : m_passes) {
    p->Execute(gfx);
  }
}

void RenderGraph::Reset() noexcept {
  assert(finalized);
  for (auto &p : m_passes) {
    p->Reset();
  }
}

void RenderGraph::AppendPass(std::unique_ptr<Pass> pass) {
  assert(!finalized);
  // validate name uniqueness
  for (const auto &p : m_passes) {
    if (pass->GetName() == p->GetName()) {
      throw RGC_EXCEPTION("Pass name already exists: " + pass->GetName());
    }
  }

  // link outputs from passes (and global outputs) to pass inputs
  LinkSinks(*pass);

  // add to container of passes
  m_passes.push_back(std::move(pass));
}

Pass &RenderGraph::FindPassByName(const std::string &name) {
  auto i = std::find_if(m_passes.begin(), m_passes.end(),
                        [&name](auto &p) { return p->GetName() == name; });
  if (i == m_passes.end()) {
    throw std::runtime_error("Can't Find the Pass with name" + name);
  }
  return **i;
}

void RenderGraph::LinkSinks(Pass &pass) {
  for (auto &si : pass.GetSinks()) {
    const auto &inputSourcePassName = si->GetPassName();

    // check check whether target source is global
    if (inputSourcePassName == "$") {
      bool bound = false;
      for (auto &source : m_globalSources) {
        if (source->GetName() == si->GetOutputName()) {
          si->Bind(*source);
          bound = true;
          break;
        }
      }
      if (!bound) {
        std::ostringstream oss;
        oss << "Output named [" << si->GetOutputName()
            << "] not found in globals";
        throw RGC_EXCEPTION(oss.str());
      }
    } else // find source from within existing passes
    {
      bool bound = false;
      for (auto &existingPass : m_passes) {
        if (existingPass->GetName() == inputSourcePassName) {
          auto &source = existingPass->GetSource(si->GetOutputName());
          si->Bind(source);
          bound = true;
          break;
        }
      }
      if (!bound) {
        std::ostringstream oss;
        oss << "Pass named [" << inputSourcePassName << "] not found";
        throw RGC_EXCEPTION(oss.str());
      }
    }
  }
}

void RenderGraph::LinkGlobalSinks() {
  for (auto &sink : m_globalSinks) {
    const auto &inputSourcePassName = sink->GetPassName();
    for (auto &existingPass : m_passes) {
      if (existingPass->GetName() == inputSourcePassName) {
        auto &source = existingPass->GetSource(sink->GetOutputName());
        sink->Bind(source);
        break;
      }
    }
  }
}

void RenderGraph::Finalize() {
  assert(!finalized);
  for (const auto &p : m_passes) {
    p->Finalize();
  }
  LinkGlobalSinks();
  finalized = true;
}

RenderQueuePass &RenderGraph::GetRenderQueue(const std::string &passName) {
  try {
    for (const auto &p : m_passes) {
      if (p->GetName() == passName) {
        return dynamic_cast<RenderQueuePass &>(*p);
      }
    }
  } catch (std::bad_cast &) {
    throw RGC_EXCEPTION(
        "In RenderGraph::GetRenderQueue, pass was not RenderQueuePass: " +
        passName);
  }
  throw RGC_EXCEPTION("In RenderGraph::GetRenderQueue, pass not found: " +
                      passName);
}
} // namespace Rgph