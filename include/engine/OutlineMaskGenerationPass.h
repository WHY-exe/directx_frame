#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "NullPixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

class Graphics;

namespace Rgph
{
	class OutlineMaskGenerationPass : public RenderQueuePass
	{
	public:
		OutlineMaskGenerationPass( Graphics& gfx,std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil",depthStencil ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil",depthStencil ) );
			AddBind( VertexShader::Resolve( gfx,L"res\\cso\\Solid_VS.cso" ) );
			AddBind( NullPixelShader::Resolve( gfx ) );
			AddBind( Stencil::Resolve( gfx,Stencil::Mod::Write ) );
			AddBind( Rasterizer::Resolve( gfx,false ) );
		}
	};
}