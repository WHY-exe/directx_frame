#pragma once
#include "BindingPass.h"
#include "BindableCommon.h"
#include "SkyBoxCbuf.h"
#include "CubeTexture.h"
#include "Stencil.h"
#include "Camera.h"
#include "Sampler.h"
#include "Cube.h"
namespace Rgph{
	class SkyBoxPass : public BindingPass
	{
	public:
		SkyBoxPass(Graphics& gfx, std::string name)
			:
			BindingPass(std::move(name))
		{
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			const std::vector<std::string> pathes = {
				"left.png",				
				"right.png",
				"top.png",				
				"bottom.png",
				"back.png",				
				"front.png"
			};
			AddBind(std::make_shared<CubeTexture>(gfx, "res\\Skybox\\", pathes));
			AddBind(Stencil::Resolve(gfx, Stencil::Mod::DepthFirst));
			AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear, true));
			AddBind(Rasterizer::Resolve(gfx, true));
			AddBind(std::make_shared<SkyBoxCbuf>(gfx));
			AddBind(PixelShader::Resolve(gfx, L"res\\cso\\Skybox_PS.cso"));
			AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			{
				auto pvs = VertexShader::Resolve(gfx, L"res\\cso\\Skybox_VS.cso");
				auto model = Cube::Make();
				model.Transform(DirectX::XMMatrixScaling(3.0f, 3.0f, 3.0f));
				const auto geometryTag = L"$cube_map";
				AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
				indicies_num = (UINT)model.indices.size();
				AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));
				AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
				AddBind(std::move(pvs));
			}
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
		}
		void BindMainCamera(const Camera& cam)
		{
			m_pCamera = &cam;
		}
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override
		{
			assert(m_pCamera);
			m_pCamera->BindtoGFX(gfx);
			BindAll(gfx);
			gfx.DrawIndexed(indicies_num);
		}
	private:
		UINT indicies_num;
		const Camera* m_pCamera = nullptr;
	};
}