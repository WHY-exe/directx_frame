#pragma once
#include "InitWin.h"
#include <Windows.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

std::wstring String2Utf8String(const std::string &str_in);
std::string Utf8String2String(const std::wstring &str_in);
std::wstring String2GBKString(const std::string &str_in);
std::string GBKString2String(const std::wstring &str_in);
template <class Iter>
void SplitStringIter(const std::string &s, const std::string &delim, Iter out) {
  if (delim.empty()) {
    *out++ = s;
  } else {
    size_t a = 0, b = s.find(delim);
    for (; b != std::string::npos;
         a = b + delim.length(), b = s.find(delim, a)) {
      *out++ = std::move(s.substr(a, b - a));
    }
    *out++ = std::move(s.substr(a, s.length() - a));
  }
}

inline std::vector<std::string> SplitString(const std::string &s,
                                            const std::string &delim) {
  std::vector<std::string> strings;
  SplitStringIter(s, delim, std::back_inserter(strings));
  return strings;
}
#define ANSI_TO_UTF8_STR(input) String2Utf8String(input)
#define UTF8_TO_ANSI_STR(input) Utf8String2String(input)
#define ANSI_TO_GBK_STR(input) String2GBKString(input)
#define GBK_TO_ANSI_STR(input) GBKString2String(input)