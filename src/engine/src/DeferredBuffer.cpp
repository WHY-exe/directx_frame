#include "DeferredBuffer.h"
#include "GfxThrowMacro.h"
void DeferredBuffer::BindAsBuffer(Graphics &gfx) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);
  GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetRenderTargets(
      buffer_num, m_RTVs->GetAddressOf(), m_DSV.Get()));
  // configure viewport
  D3D11_VIEWPORT vp = {};
  vp.Width = (float)m_width;
  vp.Height = (float)m_height;
  vp.MinDepth = 0;
  vp.MaxDepth = 1;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  // bind the view port to the pipeline
  GetContext(gfx)->RSSetViewports(1u, &vp);
}

void DeferredBuffer::Clear(Graphics &gfx,
                           std::array<float, 4> color) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);
  for (UINT i = 0; i < buffer_num; i++) {
    GFX_THROW_INFO_ONLY(
        GetContext(gfx)->ClearRenderTargetView(m_RTVs[i].Get(), color.data()));
  }
  GetContext(gfx)->ClearDepthStencilView(
      m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}
void DeferredBuffer::Clear(Graphics &gfx) noexcept(!IS_DEBUG) {
  Clear(gfx, {0.0f, 0.0f, 0.0f, 0.4f});
}
DeferredBuffer::DeferredBuffer(Graphics &gfx, UINT width, UINT height)
    : m_width(width), m_height(height) {
  IMPORT_INFOMAN(gfx);
  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
  // create texture resource
  D3D11_TEXTURE2D_DESC texDesc = {};
  texDesc.Width = width;
  texDesc.Height = height;
  texDesc.MipLevels = 1u;
  texDesc.ArraySize = 1u;
  texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  texDesc.SampleDesc.Count = 1u;
  texDesc.SampleDesc.Quality = 0u;
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  texDesc.MiscFlags = 0;
  texDesc.CPUAccessFlags = 0;
  GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, nullptr, &pTexture));
  // create render target view on the texture
  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
  rtvDesc.Format = texDesc.Format;
  rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  rtvDesc.Texture2D = D3D11_TEX2D_RTV{0};
  for (size_t i = 0; i < buffer_num; i++) {
    GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(
        pTexture.Get(), &rtvDesc, &m_RTVs[i]));
  }

  // create depth stencil view
  Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilTex;
  D3D11_TEXTURE2D_DESC depthDesc = {};
  depthDesc.Width = width;
  depthDesc.Height = height;
  depthDesc.MipLevels = 1u;
  depthDesc.ArraySize = 1u;
  depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthDesc.SampleDesc.Count = 1u;
  depthDesc.SampleDesc.Quality = 0u;
  depthDesc.Usage = D3D11_USAGE_DEFAULT;
  depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  GFX_THROW_INFO(
      GetDevice(gfx)->CreateTexture2D(&depthDesc, nullptr, &pDepthStencilTex));

  D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
  descView.Format = depthDesc.Format;
  descView.Flags = 0;
  descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  descView.Texture2D.MipSlice = 0;
  GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pDepthStencilTex.Get(),
                                                        &descView, &m_DSV));
}

DeferredBuffer::DeferredBuffer(Graphics &gfx)
    : DeferredBuffer(gfx, gfx.GetWindowWidth(), gfx.GetWindowHeight()) {}

DeferredBufferAsShaderInput::DeferredBufferAsShaderInput(Graphics &gfx,
                                                         UINT width,
                                                         UINT height, UINT slot)
    : DeferredBuffer(gfx, width, height), m_slot(slot) {
  IMPORT_INFOMAN(gfx);
  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
  // create texture resource
  D3D11_TEXTURE2D_DESC texDesc = {};
  texDesc.Width = width;
  texDesc.Height = height;
  texDesc.MipLevels = 1u;
  texDesc.ArraySize = 1u;
  texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  texDesc.SampleDesc.Count = 1u;
  texDesc.SampleDesc.Quality = 0u;
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  texDesc.MiscFlags = 0;
  texDesc.CPUAccessFlags = 0;
  GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, nullptr, &pTexture));

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1u;
  srvDesc.Texture2D.MostDetailedMip = 0u;
  for (size_t i = 0; i < buffer_num; i++)
    GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
        pTexture.Get(), &srvDesc, &m_PSRV[i]));
}

DeferredBufferAsShaderInput::DeferredBufferAsShaderInput(Graphics &gfx,
                                                         UINT slot)
    : DeferredBufferAsShaderInput(gfx, gfx.GetWindowWidth(),
                                  gfx.GetWindowHeight(), slot) {}

void DeferredBufferAsShaderInput::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);
  GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(
      m_slot, buffer_num, m_PSRV->GetAddressOf()));
}

void DeferredBufferAsShaderInput::Resize(Graphics &gfx, UINT width,
                                         UINT height) {
  *this = DeferredBufferAsShaderInput(gfx, width, height);
}

DeferredBufferAsTarget::DeferredBufferAsTarget(Graphics &gfx, UINT width,
                                               UINT height)
    : DeferredBuffer(gfx, width, height) {}

DeferredBufferAsTarget::DeferredBufferAsTarget(Graphics &gfx)
    : DeferredBuffer(gfx) {}

void DeferredBufferAsTarget::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  assert("Cannot bind OuputOnlyDeferredBuffer as shader input" && false);
}

void DeferredBufferAsTarget::Resize(Graphics &gfx, UINT width, UINT height) {
  *this = DeferredBufferAsTarget(gfx, width, height);
}
