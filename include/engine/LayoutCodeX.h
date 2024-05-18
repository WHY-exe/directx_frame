#pragma once
#include "DynamicConstantBuffer.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace DCBuf {
class LayoutCodeX {
public:
  static CookedLayout Resolve(RawLayout &&layout) noexcept(!IS_DEBUG);

private:
  static LayoutCodeX &Get_() noexcept;
  std::unordered_map<std::string, std::shared_ptr<LayoutElement>> m_elTable;
};
} // namespace DCBuf
