#pragma once
#include "DxgiInfoManager.h"
#include "InitWin.h"
#include "WinException.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <memory>

class Graphics {
  friend class GraphicsResource;

public:
  class GfxExcepion : public WinException {
  public:
    GfxExcepion(int nLine, const char *szFile, HRESULT hr,
                std::vector<std::string> v_szMsg);
    GfxExcepion(int nLine, const char *szFile, HRESULT hr);
    const char *what() const noexcept override;
    const char *GetType() const noexcept override;
    std::string GetErrorInfo() const noexcept;

  private:
    std::string m_info;
  };
  class GfxInfoOnlyException : public Exception {
  public:
    GfxInfoOnlyException(int nLine, const char *szFile,
                         std::vector<std::string> &v_szInfo);
    const char *what() const noexcept override;
    const char *GetType() const noexcept override;
    std::string GetErrorInfo() const noexcept;

  private:
    std::string m_szInfo;
  };

public:
  Graphics(HWND hWnd, int nWinWidth, int nWinHeight);
  Graphics(const Graphics &gfx) = delete;
  Graphics &operator=(const Graphics &gfx) = delete;
  ~Graphics();
  void DrawIndexed(UINT index_count);
  void SetProjection(DirectX::FXMMATRIX proj) noexcept;
  void ResetWindowSize(int WinWidth, int WinHeight) noexcept;
  int GetWindowWidth() const noexcept;
  int GetWindowHeight() const noexcept;
  void SetCamera(DirectX::FXMMATRIX cam) noexcept;
  DirectX::XMMATRIX GetCamera() const noexcept;
  DirectX::XMMATRIX GetProjection() const noexcept;
  std::shared_ptr<class RenderTarget> GetTarget();
  void BeginFrame();
  void EndFrame();

public:
  void ResizeFrameBuffer(UINT bufferWidth, UINT bufferHeight);

private:
  void RemakeRenderTarget();

public:
  bool sizeSignalDS = false;
  bool sizeSignalSM = false;
  bool sizeSignalPrj = false;

private:
  int m_winWidth;
  int m_winHeight;
  DirectX::XMMATRIX m_projection;
  DirectX::XMMATRIX m_camTransform;
#ifndef NDEBUG
  DxgiInfoManager infoManager;
#endif
  Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
  Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
  // Microsoft::WRL::ComPtr<ID3D11DeviceContext>
  // m_pDeferredContext;
  std::shared_ptr<class RenderTarget> m_pTarget;
};