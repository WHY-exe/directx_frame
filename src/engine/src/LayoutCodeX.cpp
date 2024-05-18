#include "LayoutCodeX.h"
namespace DCBuf {

CookedLayout LayoutCodeX::Resolve(RawLayout &&layout) noexcept(!IS_DEBUG) {
  auto sig = layout.GetSignature();
  auto &map = Get_().m_elTable;
  const auto i = map.find(sig);
  if (i != map.end()) {
    layout.ClearRoot();
    return {i->second};
  }
  auto result = map.insert({std::move(sig), layout.DeliverRoot()});
  return {result.first->second};
}

LayoutCodeX &LayoutCodeX::Get_() noexcept {
  static LayoutCodeX s_CodeX;
  return s_CodeX;
}
} // namespace DCBuf