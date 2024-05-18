#pragma once
#include "Exception.h"
#include "Graphics.h"
#include "InitWin.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <memory>
#include <optional>
class Window {
public:
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;
  Window(std::wstring szWinTitle, int nWidth, int nHeight,
         std::wstring szWinClass = L"DirectXFrameWork");
  ~Window();
  void SetWindowTitle(std::wstring szTitle);
  // c++17 required
  static std::optional<WPARAM> RunWindow();
  Graphics &GetGfx();

public:
  int GetWindowWidth() const;
  int GetWindowHeight() const;
  void EnableCursor() noexcept;
  void DisableCursor() noexcept;
  bool CursorEnabled() const noexcept;

public:
  Keyboard kbd;
  Mouse mouse;

private:
  static LRESULT WINAPI MsgHandlerSetUp(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam);
  static LRESULT WINAPI MsgHandlerCall(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);
  LRESULT MsgHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  void ShowCursor() noexcept;
  void HideCursor() noexcept;
  void ConfineCursor() noexcept;
  void FreeCursor() noexcept;
  void EnableImguiMouse() noexcept;
  void DisableImguiMouse() noexcept;

private:
  bool m_bEnableCursor = false;
  std::unique_ptr<Graphics> m_pGfx;
  HWND m_hWnd = nullptr;
  HINSTANCE m_hIns;
  std::wstring m_szWinClass;
  std::vector<BYTE> m_RawBuffer;
  int m_nWidth;
  int m_nHeight;

private:
  void InitWinClass();
  void InitWindow(std::wstring szWinTitile, int nWidth, int nHeight);
};
