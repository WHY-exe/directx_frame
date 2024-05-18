#include "Graphics.h"
#include "DepthStencil.h"
#include "GfxThrowMacro.h"
#include "RenderTarget.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <cmath>
#include <d3dcompiler.h>
#include <sstream>
// link the library
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

Graphics::Graphics(HWND hWnd, int nWinWidth = 0, int nWinHeight = 0)
    : m_winWidth(nWinWidth), m_winHeight(nWinHeight) {
  // create a swap chain descripter
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 0;
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  sd.Flags = 0;
  // determine wether it is on debug mod
  unsigned int swapChainCreateFlags = 0u;
#ifndef NDEBUG
  swapChainCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG
  // create device and swap chain
  INIT_GFX_EXCEPTION;
  GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, swapChainCreateFlags, nullptr,
      0, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, nullptr,
      &m_pContext));
  // create deffered context
  // GFX_THROW_INFO(m_pDevice->CreateDeferredContext(0, &m_pDeferredContext));
  Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackResource;
  GFX_THROW_INFO(
      m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackResource));
  m_pTarget = std::shared_ptr<RenderTarget>{
      new RenderTargetAsOutputTarget(*this, pBackResource.Get())};
  ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());
}

Graphics::~Graphics() { ImGui_ImplDX11_Shutdown(); }

void Graphics::DrawIndexed(UINT index_count) {
  m_pContext->DrawIndexed(index_count, 0u, 0u);
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept {
  m_projection = proj;
}

void Graphics::ResetWindowSize(int WinWidth, int WinHeight) noexcept {
  m_winWidth = WinWidth;
  m_winHeight = WinHeight;
}

int Graphics::GetWindowWidth() const noexcept { return m_winWidth; }

int Graphics::GetWindowHeight() const noexcept { return m_winHeight; }

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept {
  m_camTransform = cam;
}

void Graphics::RemakeRenderTarget() {
  INIT_GFX_EXCEPTION;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackResource;
  GFX_THROW_INFO(
      m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackResource));
  m_pTarget->Remake(*this, pBackResource.Get());
}

void Graphics::ResizeFrameBuffer(UINT bufferWidth, UINT bufferHeight) {
  INIT_GFX_EXCEPTION;
  m_pTarget->CleanUp();
  // m_pContext->ClearState();
  GFX_THROW_INFO(m_pSwapChain->ResizeBuffers(0, bufferWidth, bufferHeight,
                                             DXGI_FORMAT_UNKNOWN, 0u));
  RemakeRenderTarget();
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept {
  return m_camTransform;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept {
  return m_projection;
}

std::shared_ptr<RenderTarget> Graphics::GetTarget() { return m_pTarget; }

void Graphics::BeginFrame() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  // clearing shader inputs to prevent simultaneous in/out bind carried over
  // from prev frame
  ID3D11ShaderResourceView *const pNullTex = nullptr;
  m_pContext->PSSetShaderResources(0, 1, &pNullTex); // fullscreen input texture
  m_pContext->PSSetShaderResources(4, 1, &pNullTex); // shadow map texture
}

void Graphics::EndFrame() {
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  INIT_GFX_EXCEPTION;
#ifndef NDEBUG
  infoManager.Set();
#endif
  if (FAILED(hr = m_pSwapChain->Present(1u, 0u))) {
    if (hr == DXGI_ERROR_DEVICE_REMOVED) {
      GFX_DEVICE_REMOVED_CALL(m_pDevice->GetDeviceRemovedReason());
    } else {
      GFX_THROW_INFO(hr);
    }
  }
}
Graphics::GfxExcepion::GfxExcepion(int nLine, const char *szFile, HRESULT hr,
                                   std::vector<std::string> v_szMsg)
    : WinException(nLine, szFile, hr) {
  for (const auto &info : v_szMsg) {
    m_info += info;
    m_info.append("\n");
  }
}

Graphics::GfxExcepion::GfxExcepion(int nLine, const char *szFile, HRESULT hr)
    : WinException(nLine, szFile, hr) {}

const char *Graphics::GfxExcepion::what() const noexcept {
  std::ostringstream oss;
  oss << GetType() << std::endl
      << "[Error Code]:0x" << std::hex << GetErrorCode() << std::endl
      << "[Description]:" << TranslateErrorCode(m_ErrorCode) << std::endl
      << GetInfoString() << std::endl
#ifndef NDEBUG
      << "[DxErrInfo]:" << GetErrorInfo()
#endif // !NDEBUG
      ;
  WhatInfoBuffer = oss.str();
  return WhatInfoBuffer.c_str();
}

const char *Graphics::GfxExcepion::GetType() const noexcept {
  return "Direct3D Exception";
}

std::string Graphics::GfxExcepion::GetErrorInfo() const noexcept {
  return m_info;
}

Graphics::GfxInfoOnlyException::GfxInfoOnlyException(
    int nLine, const char *szFile, std::vector<std::string> &v_szInfo)
    : Exception(nLine, szFile) {
  for (const auto &i : v_szInfo) {
    m_szInfo += i;
    m_szInfo.push_back('\n');
  }
  if (!m_szInfo.empty()) {
    m_szInfo.pop_back();
  }
}

const char *Graphics::GfxInfoOnlyException::what() const noexcept {
  std::ostringstream oss;
  oss << GetType() << std::endl
      << "\n[Error Info]\n"
      << GetErrorInfo() << std::endl
      << std::endl
      << GetInfoString();
  WhatInfoBuffer = oss.str();
  return WhatInfoBuffer.c_str();
}

const char *Graphics::GfxInfoOnlyException::GetType() const noexcept {
  return "Graphics Info Exception";
}

std::string Graphics::GfxInfoOnlyException::GetErrorInfo() const noexcept {
  return m_szInfo;
}
