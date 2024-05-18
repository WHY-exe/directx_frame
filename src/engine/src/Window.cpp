#include "Window.h"
#include "WinException.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <cstddef>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

Window::Window(std::wstring szWinTitle, int nWidth, int nHeight,
               std::wstring szWinClass)
    : m_hIns(GetModuleHandle(nullptr)), m_szWinClass(szWinClass),
      m_nWidth(nWidth), m_nHeight(nHeight) {
  this->InitWindow(szWinTitle, nWidth, nHeight);
}

Window::~Window() {
  ImGui_ImplWin32_Shutdown();
  UnregisterClassW(m_szWinClass.c_str(), m_hIns);
  DestroyWindow(m_hWnd);
}

void Window::SetWindowTitle(std::wstring szTitle) {
  if (!SetWindowTextW(m_hWnd, szTitle.c_str())) {
    throw WND_LAST_EXCEPT();
  }
}

void Window::InitWinClass() {
  kbd.OnKeyDown(20);
  WNDCLASSEXW wcex{};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = MsgHandlerSetUp;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = m_hIns;
  wcex.hIcon = NULL;
  wcex.hIconSm = NULL;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = nullptr;
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = m_szWinClass.c_str();
  if (!RegisterClassExW(&wcex)) {
    throw WND_LAST_EXCEPT();
  }
}
void Window::InitWindow(std::wstring szWinTitile, int nWidth, int nHeight) {
  this->InitWinClass();
  RECT rectWindow = {0, 0, nWidth, nHeight};
  if (!AdjustWindowRect(&rectWindow, WS_OVERLAPPEDWINDOW, FALSE)) {
    throw WND_LAST_EXCEPT();
  }
  this->m_hWnd =
      CreateWindowExW(NULL, m_szWinClass.c_str(), szWinTitile.c_str(),
                      // window style
                      WS_OVERLAPPEDWINDOW,
                      // window position
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      // window size
                      rectWindow.right - rectWindow.left,
                      rectWindow.bottom - rectWindow.top, NULL, NULL, m_hIns,
                      this // �������װ�У����һ������һ������this������
      );
  if (this->m_hWnd == nullptr) {
    throw WND_LAST_EXCEPT();
  }
  // show window
  ShowWindow(this->m_hWnd, SW_NORMAL);
  UpdateWindow(this->m_hWnd);
  //
  ImGui_ImplWin32_Init(m_hWnd);
  // init graphics object
  m_pGfx = std::make_unique<Graphics>(m_hWnd, nWidth, nHeight);
  // ʹ��Ӳ��raw input
  // �������ƶ�����ֱ�Ӽ�¼���ǹ����ƶ�����ļ�¼��
  RAWINPUTDEVICE rid{};
  rid.usUsagePage = 0x01; // mouse page
  rid.usUsage = 0x02;     // mouse page;
  rid.hwndTarget = nullptr;
  rid.dwFlags = 0;
  if (!RegisterRawInputDevices(&rid, 1u, sizeof(rid))) {
    throw WND_LAST_EXCEPT();
  }
}
std::optional<WPARAM> Window::RunWindow() {
  MSG msg{};

  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      return msg.wParam;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return {};
}

Graphics &Window::GetGfx() { return *m_pGfx; }

int Window::GetWindowWidth() const { return m_nWidth; }

int Window::GetWindowHeight() const { return m_nHeight; }

void Window::EnableCursor() noexcept {
  m_bEnableCursor = true;
  ShowCursor();
  EnableImguiMouse();
  FreeCursor();
}

void Window::DisableCursor() noexcept {
  m_bEnableCursor = false;
  HideCursor();
  DisableImguiMouse();
  ConfineCursor();
}

bool Window::CursorEnabled() const noexcept { return m_bEnableCursor; }

LRESULT WINAPI Window::MsgHandlerSetUp(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam) {
  if (uMsg == WM_NCCREATE) {
    // ��ȡCreateWindow()API���ص�CREATESTRUCT����this��
    const CREATESTRUCT *const pCreate =
        reinterpret_cast<CREATESTRUCT *>(lParam);
    // ��ȡʵ���������ָ��(����CreateWindowʱ�����һ������)
    Window *const pWin = static_cast<Window *>(pCreate->lpCreateParams);
    if (!pWin) {
      throw WND_LAST_EXCEPT();
    }
    // ��ʵ�����Ķ����ָ���봰��ʵ�������
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWin));
    // ��ԭʼWinProc������MsgHandlerSetUp�滻ΪMsgHandlerCall
    // ��WM_NCCREATE���ý�����֮���ÿһ����Ϣ��������MsgHandlerCall����
    SetWindowLongPtr(hWnd, GWLP_WNDPROC,
                     reinterpret_cast<LONG_PTR>(&Window::MsgHandlerCall));
    return pWin->MsgHandler(hWnd, uMsg, wParam, lParam);
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI Window::MsgHandlerCall(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam) {
  // ��ȡ�봰��ʵ�����������ݣ�ʵ�����Ķ���ָ�룩
  Window *const pWin =
      reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  // ����Ϣ����ÿһ��ʵ�������MsgHandler����
  return pWin->MsgHandler(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::MsgHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
    return true;
  }
  // const auto &imio = ImGui::GetIO();
  switch (uMsg) {
  case WM_KILLFOCUS:
    kbd.ClearState();
    break;
  /****************** �����Ϣ ******************/
  case WM_LBUTTONDOWN: {
    mouse.OnLButtonDown(lParam);
    break;
  }
  case WM_LBUTTONUP: {
    mouse.OnLButtonUp(lParam);
    break;
  }
  case WM_MBUTTONDOWN: {
    mouse.OnMButtonDown(lParam);
    break;
  }
  case WM_MBUTTONUP: {
    mouse.OnMButtonUp(lParam);
    break;
  }
  case WM_RBUTTONDOWN: {
    mouse.OnRButtonDown(lParam);
    break;
  }
  case WM_RBUTTONUP: {
    mouse.OnRButtonUp(lParam);
    break;
  }
  case WM_MOUSEMOVE: {
    POINTS pt = MAKEPOINTS(lParam);
    if (pt.x > 0 && pt.y > 0 && pt.x <= m_nWidth && pt.y <= m_nHeight) {
      mouse.OnMouseMove(lParam);
      if (!mouse.IsInWindow()) {
        SetCapture(m_hWnd);
        mouse.OnMouseEnter();
      }
    } else {
      // ������������Ҽ����ڵ��״̬
      if (wParam & (MK_LBUTTON | MK_RBUTTON)) {
        mouse.OnMouseMove(lParam);
      } else {
        ReleaseCapture();
        mouse.OnMouseLeave();
      }
    }
    break;
  }

  case WM_MOUSEWHEEL: {
    mouse.OnMouseWheel(wParam);
    break;
  }
  /****************** �����Ϣ ******************/

  /****************** ������Ϣ ******************/
  case WM_SYSKEYDOWN:
  case WM_KEYDOWN:
    // ����autorepeat
    if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnable()) {
      kbd.OnKeyDown(static_cast<unsigned char>(wParam));
      // std::cout << wParam << std::endl;
    }
    break;

  case WM_SYSKEYUP:
  case WM_KEYUP:
    kbd.OnKeyUp(static_cast<unsigned char>(wParam));
    break;
  case WM_CHAR:
    kbd.OnChar(static_cast<unsigned char>(wParam));
    break;

  /****************** ������Ϣ ******************/

  /****************** raw input ��Ϣ ******************/
  case WM_INPUT: {
    UINT uRBufferSize = 0;
    if ((int)GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
                             nullptr, &uRBufferSize,
                             sizeof(RAWINPUTHEADER)) == -1) {
      break;
    }
    m_RawBuffer.resize(uRBufferSize);
    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
                        m_RawBuffer.data(), &uRBufferSize,
                        sizeof(RAWINPUTHEADER)) != uRBufferSize) {
      break;
    }
    auto &ri = reinterpret_cast<const RAWINPUT &>(*m_RawBuffer.data());
    if (ri.header.dwType == RIM_TYPEMOUSE &&
        (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0)) {
      mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
    }

    break;
  }
  /****************** raw input ��Ϣ ******************/

  /************** ���ڴ�С�ı���Ϣ **************/
  case WM_SIZE: {
    m_nWidth = LOWORD(lParam);
    m_nHeight = HIWORD(lParam);
    if (this->m_pGfx.get() && wParam != SIZE_MINIMIZED) {
      m_pGfx->ResetWindowSize(m_nWidth, m_nHeight);
      m_pGfx->sizeSignalDS = true;
      m_pGfx->sizeSignalPrj = true;
      m_pGfx->sizeSignalSM = true;
      m_pGfx->ResizeFrameBuffer(m_nWidth, m_nHeight);
    }
    break;
  }
    /************** ���ڴ�С�ı���Ϣ **************/

  case WM_CLOSE:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Window::ShowCursor() noexcept {
  while (::ShowCursor(TRUE) < 0)
    ;
}

void Window::HideCursor() noexcept {
  while (::ShowCursor(FALSE) >= 0)
    ;
}

void Window::ConfineCursor() noexcept {
  RECT crec;
  GetClientRect(m_hWnd, &crec);
  MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT *>(&crec), 2u);
  ClipCursor(&crec);
}

void Window::FreeCursor() noexcept { ClipCursor(nullptr); }

void Window::EnableImguiMouse() noexcept {
  ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImguiMouse() noexcept {
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}
