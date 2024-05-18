#pragma once
#include "Exception.h"
#include "InitWin.h"
#include <Windows.h>
#include <string>
class WinException : public Exception {
public:
  WinException(int nLine, const char *szFile, HRESULT ErrorCode);
  const char *what() const noexcept override;
  virtual const char *GetType() const noexcept override;

protected:
  static std::string TranslateErrorCode(HRESULT ErrorCode);
  HRESULT GetErrorCode() const noexcept;

protected:
  HRESULT m_ErrorCode;
};

#ifdef _DEBUG
#define INIT_WND_EXCEPT HRESULT hr
#define WND_EXCEPT(error_code) WinException(__LINE__, __FILE__, error_code)
#define WND_LAST_EXCEPT() WinException(__LINE__, __FILE__, GetLastError())
#define WND_CALL(hr_func)                                                      \
  hr = hr_func;                                                                \
  if (FAILED(hr))                                                              \
  throw WND_EXCEPT(hr)
#else
#define INIT_WND_EXCEPT
#define WND_EXCEPT(error_code) erro_code
#define WND_LAST_EXCEPT()
#define WND_CALL(hr) hr
#endif // !IS_DEBUG
