#include "GraphicsResource.h"
#include <stdexcept>
ID3D11Device *GraphicsResource::GetDevice(Graphics &gfx) noexcept {
  return gfx.m_pDevice.Get();
}
ID3D11DeviceContext *GraphicsResource::GetContext(Graphics &gfx) noexcept {
  return gfx.m_pContext.Get();
}

DxgiInfoManager &GraphicsResource::GetInfoMan(Graphics &gfx) {
#ifndef NDEBUG
  return gfx.infoManager;
#else
  throw std::logic_error(
      "You can only access the infoManager in the debug mode");
#endif // !NDEBUG
}
