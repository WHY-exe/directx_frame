#include "CubeTexture.h"
#include "DepthStencil.h"
#include "GfxThrowMacro.h"
CubeTexture::CubeTexture(Graphics &gfx, const std::string &base_path,
                         const std::vector<std::string> &pathes, UINT slot)
    : m_slot(slot) {
  assert(pathes.size() == 6);
  IMPORT_INFOMAN(gfx);
  std::vector<Surface> surs;
  for (size_t i = 0; i < pathes.size(); i++) {
    surs.emplace_back(Surface(base_path + pathes[i]));
  }

  D3D11_TEXTURE2D_DESC texDesc;
  texDesc.Width = surs[0].GetWidth();   // in this case, we expect the all image
                                        // have the same size
  texDesc.Height = surs[0].GetHeight(); // after all, it's cube texture
  texDesc.MipLevels = 1;
  texDesc.ArraySize = 6;
  texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  texDesc.SampleDesc.Count = 1;
  texDesc.SampleDesc.Quality = 0;
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  texDesc.CPUAccessFlags = 0;
  texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

  D3D11_SUBRESOURCE_DATA sd[6];
  for (size_t i = 0; i < 6; i++) {
    sd[i].pSysMem = surs[i].GetBufferPtr();
    sd[i].SysMemPitch = (UINT)surs[i].GetBytePitch();
    sd[i].SysMemSlicePitch = 0;
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTex;
  GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, sd, &pTex));
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = texDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  srvDesc.Texture2D.MostDetailedMip = 0;
  srvDesc.Texture2D.MipLevels = 1;
  GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTex.Get(), &srvDesc,
                                                          &m_pCubeTex));
}

void CubeTexture::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);
  GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(
      m_slot, 1, m_pCubeTex.GetAddressOf()));
}

DepthCubeTexure::DepthCubeTexure(Graphics &gfx, UINT size, UINT slot)
    : m_slot(slot) {
  IMPORT_INFOMAN(gfx);
  D3D11_TEXTURE2D_DESC texDesc;
  texDesc.Width =
      size; // in this case, we expect the all image have the same size
  texDesc.Height = size; // after all, it's cube texture
  texDesc.MipLevels = 1;
  texDesc.ArraySize = 6;
  texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
  texDesc.SampleDesc.Count = 1;
  texDesc.SampleDesc.Quality = 0;
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
  texDesc.CPUAccessFlags = 0;
  texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> pTex;
  GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, nullptr, &pTex));

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  srvDesc.Texture2D.MostDetailedMip = 0;
  srvDesc.Texture2D.MipLevels = 1;
  GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTex.Get(), &srvDesc,
                                                          &m_pCubeTex));
  for (UINT i = 0; i < 6; i++) {
    m_depthTargets.push_back(
        std::make_shared<DepthStencilAsTraget>(gfx, pTex, i));
  }
}

void DepthCubeTexure::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  IMPORT_INFOMAN(gfx);
  GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(
      m_slot, 1, m_pCubeTex.GetAddressOf()));
}

std::shared_ptr<DepthStencilAsTraget>
DepthCubeTexure::GetDepthTarget(size_t index) noexcept {
  return m_depthTargets[index];
}
