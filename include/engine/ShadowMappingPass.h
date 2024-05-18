#pragma once
#include "RenderQueuePass.h"
#include "DepthStencil.h"
#include "Stencil.h"
#include "Signal.h"
#include "Blender.h"
#include "VertexShader.h"
#include "NullPixelShader.h"
#include "Camera.h"
#include "ShadowRasterizer.h"
#include "CubeTexture.h"
#include "Viewport.h"
#include "MathTool.h"
namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		ShadowMappingPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			m_pDCTex = std::make_shared<DepthCubeTexure>(gfx, size, 4u);
			AddBind(VertexShader::Resolve(gfx, L".\\res\\cso\\Shadow_VS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mod::Off));
			AddBind(Blender::Resolve(gfx, false));
			AddBind(std::make_shared<Viewport>(gfx, size, size));
			AddBindSink<Bindable>("shadowRasterizer");
			RegisterSource(DirectBindableSource<DepthCubeTexure>::Make("ShadowMap", m_pDCTex));
			DirectX::XMStoreFloat4x4(
				&m_projection,
				DirectX::XMMatrixPerspectiveFovLH(
					PI / 2.0f, 1.0f, 0.5f, 3000.0f
				)
			);
			// +x
			DirectX::XMStoreFloat3(&m_camDirections[0], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&m_camUpDirections[0], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// -x
			DirectX::XMStoreFloat3(&m_camDirections[1], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&m_camUpDirections[1], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// +y
			DirectX::XMStoreFloat3(&m_camDirections[2], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&m_camUpDirections[2], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			// -y
			DirectX::XMStoreFloat3(&m_camDirections[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&m_camUpDirections[3], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			// +z
			DirectX::XMStoreFloat3(&m_camDirections[4], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			DirectX::XMStoreFloat3(&m_camUpDirections[4], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// -z
			DirectX::XMStoreFloat3(&m_camDirections[5], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			DirectX::XMStoreFloat3(&m_camUpDirections[5], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			SetDepthBuffer(m_pDCTex->GetDepthTarget(0));

		}
		void BindCamera(std::shared_ptr<Camera> cam)
		{
			m_pShadowCamera = std::move(cam);
		}
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override
		{
			using namespace DirectX;
			auto camPos = m_pShadowCamera->GetPos();
			const auto cam_pos_vec = XMLoadFloat3(&camPos);
			gfx.SetProjection(XMLoadFloat4x4(&m_projection));
			for (UINT i = 0; i < 6; i++)
			{
				auto d = m_pDCTex->GetDepthTarget(i);
				d->Clear(gfx);
				SetDepthBuffer(std::move(d));
				const auto lookAt = cam_pos_vec + XMLoadFloat3(&m_camDirections[i]);
				gfx.SetCamera(XMMatrixLookAtLH(cam_pos_vec, lookAt, XMLoadFloat3(&m_camUpDirections[i])));
				RenderQueuePass::Execute(gfx);
			}
		}
		//void DumpShadowMap(Graphics& gfx, const std::string& path)
		//{
		//	depthStencil->ToSurface(gfx).SaveAsFile(path);
		//}
	private:
		void SetDepthBuffer(std::shared_ptr<DepthStencil> ds) const
		{
			const_cast<ShadowMappingPass*>(this)->depthStencil = std::move(ds);
		}
		static constexpr UINT size = 1000u;
		std::shared_ptr<Camera> m_pShadowCamera;
		std::shared_ptr<DepthCubeTexure> m_pDCTex;
		DirectX::XMFLOAT4X4 m_projection;
		std::array<DirectX::XMFLOAT3, 6> m_camDirections;
		std::array<DirectX::XMFLOAT3, 6> m_camUpDirections;
	};

}