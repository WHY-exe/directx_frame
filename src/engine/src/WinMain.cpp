#include "App.h"
#include "StrManager.h"
#include <sstream>

// standar win32 windows program
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
  // ���ӿ���̨����
  // ADD_CONSOLE();
  try {
    return (int)App{}.Run();
  } catch (const Exception &e) {
    const std::wstring output_msg = ANSI_TO_GBK_STR(e.what());
    const std::wstring output_type = ANSI_TO_GBK_STR(e.GetType());
    MessageBoxW(nullptr, output_msg.c_str(), output_type.c_str(),
                MB_OKCANCEL | MB_ICONASTERISK);
  } catch (const std::exception &e) {
    const std::wstring output_msg = ANSI_TO_GBK_STR(e.what());
    MessageBoxW(nullptr, output_msg.c_str(), L"Standard Error",
                MB_OKCANCEL | MB_ICONASTERISK);
  } catch (...) {
    MessageBoxW(nullptr, L"No available information", L"UnKnown Error",
                MB_OKCANCEL | MB_ICONASTERISK);
  }
  // �رտ���̨
  // FREE_CONSOLE();
  return -1;
}
