#include "RenderTarget.h"
#include "DepthStencil.h"
#include "GfxThrowMacro.h"
RenderTarget::RenderTarget(Graphics &gfx, UINT width, UINT height)
    : m_uWidth(width), m_uHeight(height) {
  IMPORT_INFOMAN(gfx);
  // create texture resource
  D3D11_TEXTURE2D_DESC texDesc = {};
  texDesc.Width = width;
  texDesc.Height = height;
  texDesc.MipLevels = 1u;
  texDesc.ArraySize = 1u;
  texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  texDesc.SampleDesc.Count = 1u;
  texDesc.SampleDesc.Quality = 0u;
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  texDesc.MiscFlags = 0;
  texDesc.CPUAccessFlags = 0;
  GFX_THROW_INFO(
      GetDevice(gfx)->CreateTexture2D(&texDesc, nullptr, &m_pTexture));
  // create render target view on the texture
  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
  rtvDesc.Format = texDesc.Format;
  rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  rtvDesc.Texture2D = D3D11_TEX2D_RTV{0};
  GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(m_pTexture.Get(),
                                                        &rtvDesc, &m_pTarget));
}

RenderTarget::RenderTarget(Graphics &gfx, ID3D11Texture2D *pTexture) {
  IMPORT_INFOMAN(gfx);
  D3D11_TEXTURE2D_DESC texDesc = {};
  pTexture->GetDesc(&texDesc);
  m_uWidth = texDesc.Width;
  m_uHeight = texDesc.Height;
  m_pTexture = pTexture;
  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
  rtvDesc.Format = texDesc.Format;
  rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  rtvDesc.Texture2D = D3D11_TEX2D_RTV{0};
  GFX_THROW_INFO(
      GetDevice(gfx)->CreateRenderTargetView(pTexture, &rtvDesc, &m_pTarget));
}

void RenderTarget::BindAsBuffer(Graphics &gfx) noexcept(!IS_DEBUG) {
  GetContext(gfx)->OMSetRenderTargets(1u, m_pTarget.GetAddressOf(), nullptr);
  // configure viewport
  D3D11_VIEWPORT vp = {};
  vp.Width = (float)m_uWidth;
  vp.Height = (float)m_uHeight;
  vp.MinDepth = 0;
  vp.MaxDepth = 1;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  // bind the view port to the pipeline
  GetContext(gfx)->RSSetViewports(1u, &vp);
}

void RenderTarget::BindAsBuffer(
    Graphics &gfx, BufferResource *depthStencil) noexcept(!IS_DEBUG) {
  assert(dynamic_cast<DepthStencil *>(depthStencil) != nullptr);
  BindAsBuffer(gfx, static_cast<DepthStencil *>(depthStencil));
}

void RenderTarget::BindAsBuffer(Graphics &gfx,
                                DepthStencil *ds) noexcept(!IS_DEBUG) {
  GetContext(gfx)->OMSetRenderTargets(1u, m_pTarget.GetAddressOf(),
                                      ds ? ds->m_pDSV.Get() : nullptr);
  // configure viewport
  D3D11_VIEWPORT vp = {};
  vp.Width = (float)m_uWidth;
  vp.Height = (float)m_uHeight;
  vp.MinDepth = 0;
  vp.MaxDepth = 1;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  // bind the view port to the pipeline
  GetContext(gfx)->RSSetViewports(1u, &vp);
}

void RenderTarget::Remake(Graphics &gfx, ID3D11Texture2D *pTexture) {
  IMPORT_INFOMAN(gfx);
  D3D11_TEXTURE2D_DESC texDesc = {};
  pTexture->GetDesc(&texDesc);
  m_uWidth = texDesc.Width;
  m_uHeight = texDesc.Height;
  m_pTexture = pTexture;
  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
  rtvDesc.Format = texDesc.Format;
  rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  rtvDesc.Texture2D = D3D11_TEX2D_RTV{0};
  GFX_THROW_INFO(
      GetDevice(gfx)->CreateRenderTargetView(pTexture, &rtvDesc, &m_pTarget));
}

UINT RenderTarget::GetWidth() const noexcept { return m_uWidth; }

UINT RenderTarget::GetHeight() const noexcept { return m_uHeight; }

void RenderTarget::CleanUp() noexcept(!IS_DEBUG) {
  m_pTarget.Reset();
  m_pTexture.Reset();
}

void RenderTarget::Clear(Graphics &gfx) noexcept(!IS_DEBUG) {
  // when there are several render targets, be careful about the alpha channel
  Clear(gfx, {0.0f, 0.0f, 0.0f, 0.4f});
}

void RenderTarget::Clear(Graphics &gfx,
                         std::array<float, 4> color) noexcept(!IS_DEBUG) {
  GetContext(gfx)->ClearRenderTargetView(m_pTarget.Get(), color.data());
}

RenderTargetAsShaderTexture::RenderTargetAsShaderTexture(Graphics &gfx,
                                                         UINT width,
                                                         UINT height, UINT slot)
    : RenderTarget(gfx, width, height), m_slot(slot) {
  IMPORT_INFOMAN(gfx);
  // create shader resource view on the texture
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1u;
  srvDesc.Texture2D.MostDetailedMip = 0u;
  GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
      m_pTexture.Get(), &srvDesc, &m_pPSTextureView));
}

RenderTargetAsShaderTexture::RenderTargetAsShaderTexture(
    Graphics &gfx, ID3D11Texture2D *pTexture, UINT slot)
    : RenderTarget(gfx, pTexture), m_slot(slot) {}

Surface
RenderTargetAsShaderTexture::ToSurface(Graphics &gfx) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);

  // creating a temp texture compatible with the source, but with CPU read
  // access
  Microsoft::WRL::ComPtr<ID3D11Resource> pResSource;
  m_pPSTextureView->GetResource(&pResSource);
  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexSource;
  pResSource.As(&pTexSource);
  D3D11_TEXTURE2D_DESC textureDesc;
  pTexSource->GetDesc(&textureDesc);
  textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  textureDesc.Usage = D3D11_USAGE_STAGING;
  textureDesc.BindFlags = 0;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexTemp;
  GFX_THROW_INFO(
      GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexTemp));

  // copy texture contents
  GFX_THROW_INFO_ONLY(
      GetContext(gfx)->CopyResource(pTexTemp.Get(), pTexSource.Get()));

  // create Surface and copy from temp texture to it
  const auto width = GetWidth();
  const auto height = GetHeight();
  Surface s{width, height};
  D3D11_MAPPED_SUBRESOURCE msr = {};
  GFX_THROW_INFO(GetContext(gfx)->Map(pTexTemp.Get(), 0,
                                      D3D11_MAP::D3D11_MAP_READ, 0, &msr));
  auto pSrcBytes = static_cast<const char *>(msr.pData);
  for (unsigned int y = 0; y < height; y++) {
    auto pSrcRow =
        reinterpret_cast<const Color *>(pSrcBytes + msr.RowPitch * size_t(y));
    for (unsigned int x = 0; x < width; x++) {
      s.PutPixel(x, y, *(pSrcRow + x));
    }
  }
  GFX_THROW_INFO_ONLY(GetContext(gfx)->Unmap(pTexTemp.Get(), 0));

  return s;
}

void RenderTargetAsShaderTexture::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);
  GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(
      m_slot, 1u, m_pPSTextureView.GetAddressOf()));
}

void RenderTargetAsShaderTexture::Resize(Graphics &gfx, UINT width,
                                         UINT height) {
  *this = RenderTargetAsShaderTexture(gfx, width, height, m_slot);
}

void RenderTargetAsOutputTarget::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  assert("Cannot bind OuputOnlyRenderTarget as shader input" && false);
}

RenderTargetAsOutputTarget::RenderTargetAsOutputTarget(Graphics &gfx,
                                                       UINT width, UINT height)
    : RenderTarget(gfx, width, height) {}

RenderTargetAsOutputTarget::RenderTargetAsOutputTarget(
    Graphics &gfx, ID3D11Texture2D *pTexture)
    : RenderTarget(gfx, pTexture) {}

void RenderTargetAsOutputTarget::Resize(Graphics &gfx, UINT width,
                                        UINT height) {
  *this = RenderTargetAsOutputTarget(gfx, width, height);
}
