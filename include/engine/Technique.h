#pragma once
#include "Step.h"
#include "Probe.h"
#include "RenderGraph.h"
#include <vector>
class Technique
{
public:
	Technique(const std::string& name, size_t channel) noexcept;
	void Submit(const class Drawable& d, size_t channelFliter) const noexcept;
	void AddStep(Step& step) noexcept;
	void SetStepActive(UINT step_index, bool active) noexcept;
	void SetActiveState(bool is_active) noexcept;
	bool IsActive() const noexcept;
	void Accept(Probe& probe) noexcept;
	void Link(Rgph::RenderGraph& rg) noexcept(!IS_DEBUG);
	const std::string& GetTechName() const noexcept;
private:
	std::string m_Name;
	bool m_activate = true;
	std::vector<Step> m_Steps;
	size_t m_channel;
};
