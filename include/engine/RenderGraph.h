#pragma once
#include <string>
#include <vector>
#include <memory>

class Graphics;
class RenderTarget;
class DepthStencil;

namespace Rgph
{
	class Pass;
	class RenderQueuePass;
	class Source;
	class Sink;

	class RenderGraph
	{
	public:
		RenderGraph( Graphics& gfx );
		~RenderGraph();
		void Execute( Graphics& gfx ) noexcept(!IS_DEBUG);
		void Reset() noexcept;
		RenderQueuePass& GetRenderQueue( const std::string& passName );
	protected:
		void SetSinkTarget( const std::string& sinkName,const std::string& target );
		void AddGlobalSource( std::unique_ptr<Source> );
		void AddGlobalSink( std::unique_ptr<Sink> );
		void Finalize();
		void AppendPass( std::unique_ptr<Pass> pass );
		Pass& FindPassByName(const std::string& name);
	private:
		void LinkSinks( Pass& pass );
		void LinkGlobalSinks();
	private:
		std::vector<std::unique_ptr<Pass>> m_passes;
		std::vector<std::unique_ptr<Source>> m_globalSources;
		std::vector<std::unique_ptr<Sink>> m_globalSinks;
		std::shared_ptr<RenderTarget> m_backBufferTarget;
		std::shared_ptr<DepthStencil> m_masterDepth;
		bool finalized = false;
	};
}