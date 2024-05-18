#include "StrManager.h"
std::wstring String2Utf8String(const std::string &str_in) {
  const char *c_str_in = str_in.c_str();
  int len_str_in = (int)str_in.length();
  int str_len =
      MultiByteToWideChar(CP_UTF8, 0u, c_str_in, len_str_in, nullptr, 0);
  wchar_t *output_buffer = new wchar_t[(size_t)str_len + 1u];
  MultiByteToWideChar(CP_UTF8, 0u, c_str_in, len_str_in, output_buffer,
                      str_len);
  output_buffer[str_len] = '\0';
  std::wstring wszResult = std::wstring(output_buffer);
  delete[] output_buffer;
  return wszResult;
}

std::string Utf8String2String(const std::wstring &str_in) {
  const wchar_t *c_str_in = str_in.c_str();
  int str_len =
      WideCharToMultiByte(CP_UTF8, 0u, c_str_in, -1, nullptr, 0, nullptr, 0);
  char *output_buffer = new char[(size_t)str_len + 1u];
  WideCharToMultiByte(CP_UTF8, 0u, c_str_in, -1, output_buffer, str_len,
                      nullptr, nullptr);
  output_buffer[str_len] = '\0';
  std::string wszResult = std::string(output_buffer);
  delete[] output_buffer;
  return wszResult;
}

std::wstring String2GBKString(const std::string &str_in) {
  const char *c_str_in = str_in.c_str();
  int len_str_in = (int)str_in.length();
  int str_len =
      MultiByteToWideChar(CP_ACP, 0u, c_str_in, len_str_in, nullptr, 0);
  wchar_t *output_buffer = new wchar_t[(size_t)str_len + 1u];
  MultiByteToWideChar(CP_ACP, 0u, c_str_in, len_str_in, output_buffer, str_len);
  output_buffer[str_len] = '\0';
  std::wstring wszResult = std::wstring(output_buffer);
  delete[] output_buffer;
  return wszResult;
}

std::string GBKString2String(const std::wstring &str_in) {
  const wchar_t *c_str_in = str_in.c_str();
  int str_len =
      WideCharToMultiByte(CP_ACP, 0u, c_str_in, -1, nullptr, 0, nullptr, 0);
  char *output_buffer = new char[(size_t)str_len + 1u];
  WideCharToMultiByte(CP_ACP, 0u, c_str_in, -1, output_buffer, str_len, nullptr,
                      nullptr);
  output_buffer[str_len] = '\0';
  std::string wszResult = std::string(output_buffer);
  delete[] output_buffer;
  return wszResult;
}
