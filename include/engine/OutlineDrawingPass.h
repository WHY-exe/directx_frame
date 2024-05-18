#pragma once
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

namespace Rgph
{
	class OutlineDrawingPass : public RenderQueuePass
	{
	public:
		OutlineDrawingPass( Graphics& gfx,std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget",renderTarget ) );
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil",depthStencil ) );
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "renderTarget",renderTarget ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil",depthStencil ) );
			AddBind( VertexShader::Resolve( gfx,L"res\\cso\\Solid_VS.cso" ) );
			AddBind( PixelShader::Resolve( gfx,L"res\\cso\\Solid_PS.cso" ) );
			AddBind( Stencil::Resolve( gfx,Stencil::Mod::Mask ) );
			AddBind( Rasterizer::Resolve( gfx,false ) );
		}
	};
}