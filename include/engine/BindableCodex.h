#pragma once
#include "Bindable.h"
#include <memory>
#include <type_traits>
#include <unordered_map>
class CodeX {
public:
  template <class T, typename... Params>
  static std::shared_ptr<T> Resolve(Graphics &gfx,
                                    Params &&...p) noexcept(!IS_DEBUG) {
    static_assert(std::is_base_of<Bindable, T>::value,
                  "Can only Resolve classes derived from Bindable");
    return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
  }

private:
  template <class T, typename... Params>
  std::shared_ptr<T> Resolve_(Graphics &gfx,
                              Params &&...p) noexcept(!IS_DEBUG) {
    auto key = T::GenUID(std::forward<Params>(p)...);
    auto i = m_binds.find(key);
    if (i == m_binds.end()) {
      auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
      m_binds[bind->GetUID()] = bind;
      return bind;
    } else {
      return std::static_pointer_cast<T>(i->second);
    }
  }
  static CodeX &Get() {
    static CodeX codeX;
    return codeX;
  }

private:
  std::unordered_map<std::wstring, std::shared_ptr<Bindable>> m_binds;
};
