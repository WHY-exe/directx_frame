#pragma once
#include "Bindable.h"
#include "IndexBuffer.h"
#include <map>
#include <memory>
#include <DirectXMath.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderGraph.h"
#include "Probe.h"
#include "Topology.h"
#include "Technique.h"
class Drawable
{
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
	void Bind(Graphics& gfx) const noexcept;
	void Accept(Probe& probe) noexcept;
	void LinkTechniques(Rgph::RenderGraph& rg) noexcept(!IS_DEBUG);
	void Submit(size_t channel) const noexcept;
	UINT GetIndexSize() const noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
protected:
	void AddTechnique(Technique& tech) noexcept;
	// be careful about this
	void SetEssentialBind(size_t index, std::shared_ptr<Bindable> bind) noexcept;
	void AddEssentialBind(std::shared_ptr<Bindable> bind) noexcept;
private:
	const IndexBuffer* m_pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> m_essential_binds;
	std::vector<Technique> m_Techs;
};

