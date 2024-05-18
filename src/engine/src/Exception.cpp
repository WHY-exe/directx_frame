#include "Exception.h"
#include <sstream>
Exception::Exception(int nLine, const char *szFile, std::string szInfo)
    : m_nLine(nLine), m_szFile(szFile), WhatInfoBuffer(std::move(szInfo)) {}

Exception::Exception(int nLine, const char *szFile)
    : m_nLine(nLine), m_szFile(szFile) {}

const char *Exception::what() const noexcept {
  std::ostringstream oss;
  oss << GetType() << std::endl << GetInfoString();
  WhatInfoBuffer = oss.str();
  return WhatInfoBuffer.c_str();
}

const char *Exception::GetType() const noexcept { return "Base Exception"; }

int Exception::GetLine() const noexcept { return m_nLine; }

const std::string &Exception::GetFile() const noexcept { return m_szFile; }

std::string Exception::GetInfoString() const noexcept {
  std::ostringstream oss;
  oss << "[File]:" << m_szFile << std::endl
      << "[Line]:" << m_nLine << std::endl;

  return oss.str();
}
