#include "WinException.h"
#include <sstream>
WinException::WinException(int nLine, const char *szFile, HRESULT ErrorCode)
    : Exception(nLine, szFile), m_ErrorCode(ErrorCode) {}

const char *WinException::what() const noexcept {
  std::ostringstream oss;
  oss << GetType() << std::endl
      << "[ErrorCode]:" << GetErrorCode() << std::endl
      << "[Description]:" << TranslateErrorCode(m_ErrorCode) << std::endl
      << GetInfoString();
  WhatInfoBuffer = oss.str();
  return WhatInfoBuffer.c_str();
}

const char *WinException::GetType() const noexcept {
  return "Windows Exception";
}

std::string WinException::TranslateErrorCode(HRESULT ErrorCode) {
  char *pMsgBuffer = nullptr;
  DWORD nMsgLen = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPSTR>(&pMsgBuffer), 0, nullptr);
  if (nMsgLen == 0) {
    return "undefine error code";
  }
  std::string szResult = pMsgBuffer;
  // LocalFree(pMsgBuffer);
  return szResult;
}

HRESULT WinException::GetErrorCode() const noexcept { return m_ErrorCode; }
