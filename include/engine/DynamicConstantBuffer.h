#pragma once
#include "InitWin.h"
#include <DirectXMath.h>
#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>
#define LEAF_ELEMENT_TYPES                                                     \
  X(Float4)                                                                    \
  X(Float3)                                                                    \
  X(Float2)                                                                    \
  X(Float)                                                                     \
  X(Integer)                                                                   \
  X(Matrix)                                                                    \
  X(Bool)

namespace DCBuf {
enum DataType {
#define X(el) el,
  LEAF_ELEMENT_TYPES
#undef X
      Struct,
  // if you want to use arry along width other basic data,
  // make sure the basic data type is added before array type
  Array,
  Empty
};

template <DataType T> struct Map {
  static constexpr bool valid = false;
};
template <> struct Map<Float4> {
  using SysType = DirectX::XMFLOAT4;
  static constexpr size_t hlsl_size = sizeof(SysType);
  static constexpr const char *code = "F4";
  static constexpr bool valid = true;
};
template <> struct Map<Float3> {
  using SysType = DirectX::XMFLOAT3;
  static constexpr size_t hlsl_size = sizeof(SysType);
  static constexpr const char *code = "F3";
  static constexpr bool valid = true;
};
template <> struct Map<Float2> {
  using SysType = DirectX::XMFLOAT2;
  static constexpr size_t hlsl_size = sizeof(SysType);
  static constexpr const char *code = "F2";
  static constexpr bool valid = true;
};
template <> struct Map<Float> {
  using SysType = float;
  static constexpr size_t hlsl_size = sizeof(SysType);
  static constexpr const char *code = "F1";
  static constexpr bool valid = true;
};
template <> struct Map<Integer> {
  using SysType = int;
  static constexpr size_t hlsl_size = sizeof(SysType);
  static constexpr const char *code = "INT";
  static constexpr bool valid = true;
};
template <> struct Map<Matrix> {
  using SysType = DirectX::XMFLOAT4X4;
  static constexpr size_t hlsl_size = sizeof(SysType);
  static constexpr const char *code = "M4x4";
  static constexpr bool valid = true;
};
template <> struct Map<Bool> {
  using SysType = bool;
  static constexpr size_t hlsl_size = sizeof(BOOL);
  static constexpr const char *code = "BL";
  static constexpr bool valid = true;
};
#define X(el)                                                                  \
  static_assert(Map<el>::valid, "Missing map implementation for" #el);
LEAF_ELEMENT_TYPES
#undef X
template <typename T> struct ReverseMap {
  static constexpr bool valid = false;
};
#define X(el)                                                                  \
  template <> struct ReverseMap<Map<el>::SysType> {                            \
    static constexpr DataType type = el;                                       \
    static constexpr bool valid = true;                                        \
  };
LEAF_ELEMENT_TYPES
#undef X
class LayoutElement {
private:
  struct ExtraDataBase {
    virtual ~ExtraDataBase() = default;
  };
  friend class RawLayout;
  friend struct ExtraData;

public:
  std::string GetSignature() const noexcept(!IS_DEBUG);
  bool Exists() const noexcept;
  std::pair<size_t, const LayoutElement *>
  CaculateIndexingOffset(size_t index, size_t offset) const noexcept(!IS_DEBUG);
  LayoutElement &operator[](const std::string &key) noexcept(!IS_DEBUG);
  // [] only works for Structs; access member (child node in tree) by name
  const LayoutElement &operator[](const std::string &key) const
      noexcept(!IS_DEBUG);
  // T() only works for Arrays; gets the array type layout object
  // needed to further configure an array's type
  LayoutElement &T() noexcept(!IS_DEBUG);
  const LayoutElement &T() const noexcept(!IS_DEBUG);
  size_t GetOffsetBegin() const noexcept;
  size_t GetOffsetEnd() const noexcept;
  size_t GetSizeInBytes() const noexcept;
  LayoutElement &Add(const std::string &name, DataType dt) noexcept(!IS_DEBUG);
  template <DataType T>
  LayoutElement &Add(const std::string &name) noexcept(!IS_DEBUG) {
    return Add(std::move(name), T);
  }
  LayoutElement &Set(size_t size, DataType dt) noexcept(!IS_DEBUG);
  template <DataType T> LayoutElement &Set(size_t size) noexcept(!IS_DEBUG) {
    return Set(size, T);
  }
  template <typename T> size_t Resolve() const noexcept(!IS_DEBUG) {
    switch (ReverseMap<T>::type) {
#define X(el)                                                                  \
  case el:                                                                     \
    assert(typeid(Map<el>::SysType) == typeid(T));                             \
    return *m_offset;
      LEAF_ELEMENT_TYPES
#undef X
    default:
      assert("Tried to resolve non-leaf elements");
      return 0;
    }
  }

private:
  LayoutElement() = default;
  LayoutElement(DataType dt) noexcept(!IS_DEBUG);
  // sets all offsets for element and subelements, prepending padding when
  // necessary returns offset directly after this element
  size_t Finalize(size_t offsetIn) noexcept(!IS_DEBUG);
  size_t FinalizeForStruct(size_t offsetIn) noexcept(!IS_DEBUG);
  size_t FinalizeForArray(size_t offsetIn) noexcept(!IS_DEBUG);
  std::string GetSignatureForStruct() const noexcept;
  std::string GetSignatureForArray() const noexcept;
  // return singleton instance of empty layout element
  static LayoutElement &GetEmptyElement() noexcept;
  static size_t AdvanceToBoundary(size_t offset) noexcept;
  static bool CrossBoundary(size_t offset, size_t size) noexcept;
  static size_t AdvanceIfCrossBoundary(size_t offset, size_t size) noexcept;
  static bool ValidateSymbolName(const std::string &name) noexcept;

private:
  std::optional<size_t> m_offset;
  DataType m_type = Empty;
  std::unique_ptr<ExtraDataBase> m_pExtraData;
};
// the layout class serves as a shell to hold the root of the LayoutElement tree
// client does not create LayoutElements directly, create a raw layout and then
// use it to access the elements and add on from there. When building is done,
// raw layout is moved to Codex (usually via Buffer::Make), and the internal
// layout element tree is "delivered" (finalized and moved out). Codex returns a
// baked layout, which the buffer can then use to initialize itself. Baked
// layout can also be used to directly init multiple Buffers. Baked layouts are
// conceptually immutable. Base Layout class cannot be constructed.
class Layout {
  friend class LayoutCodex;
  friend class Buffer;

public:
  size_t GetSizeInBytes() const noexcept;
  std::string GetSignature() const noexcept;

protected:
  Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;
  std::shared_ptr<LayoutElement> m_pRoot;
};
// Raw layout represents a layout that has not yet been finalized and registered
// structure can be edited by adding layout nodes
class RawLayout : public Layout {
  friend class LayoutCodeX;

public:
  RawLayout() noexcept;
  LayoutElement &operator[](const std::string &key) noexcept;
  template <DataType Type>
  LayoutElement &Add(const std::string &key) noexcept(!IS_DEBUG) {
    return m_pRoot->Add<Type>(key);
  }

private:
  void ClearRoot() noexcept;
  std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
};
// Cooked layout represents a layout that has  been finalized and registered
// structure can't be edited
class CookedLayout : public Layout {
  friend class LayoutCodeX;
  friend class Buffer;

public:
  const LayoutElement &operator[](const std::string &key) const
      noexcept(!IS_DEBUG);
  std::shared_ptr<LayoutElement> ShareRoot() const noexcept;

private:
  CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept;
  std::shared_ptr<LayoutElement> RelinquishRoot() const noexcept;
};

// proxy type that is emitted when keying/indexing into a Buffer
// implement conversions/assignment that allows manipulation of the
// raw bytes of the Buffer. This version is const, only supports reading
// Refs can be further keyed/indexed to traverse the layout structure
class ConstElementRef {
  friend class Buffer;
  friend class ElementRef;

public:
  // this is a proxy type emitted when you use addressof& on the Ref
  // it allows conversion to pointer type, useful for using Buffer
  // elements with ImGui widget functions etc.
  class Ptr {
    friend class ConstElementRef;

  public:
    template <typename T> operator T *() const noexcept(!IS_DEBUG) {
      static_assert(ReverseMap<std::remove_const_t<T>>::valid,
                    "Unsupprted Sysype used in pointer conversion");
      return &static_cast<const T &>(*m_ref);
    }

  private:
    Ptr(const ConstElementRef *ref) noexcept;
    const ConstElementRef *m_ref;
  };

public:
  bool Exists() const noexcept;
  ConstElementRef operator[](const std::string &key) const noexcept(!IS_DEBUG);
  ConstElementRef operator[](size_t index) const noexcept(!IS_DEBUG);
  Ptr operator&() const noexcept(!IS_DEBUG);
  template <typename T> operator const T &() const noexcept(!IS_DEBUG) {
    static_assert(ReverseMap<std::remove_const_t<T>>::valid,
                  "Unsupported SysType used in conversion");
    return reinterpret_cast<const T *>(m_pByte + m_offset +
                                       m_pLayout->Resolve<T>());
  }

private:
  ConstElementRef(size_t offset, const char *pByte,
                  const LayoutElement *playout);
  size_t m_offset;
  const char *m_pByte;
  const LayoutElement *m_pLayout;
};
// proxy type that is emitted when keying/indexing into a Buffer
// implement conversions/assignment that allows manipulation of the
// raw bytes of the Buffer.
class ElementRef {
  friend class Buffer;

public:
  class Ptr {
    friend class ElementRef;

  public:
    template <typename T> operator T *() const noexcept(!IS_DEBUG) {
      static_assert(ReverseMap<std::remove_const_t<T>>::valid,
                    "Unsupported Systype used in pointer conversion");
      return &static_cast<T &>(*m_ref);
    }

  private:
    Ptr(ElementRef *ref) noexcept;
    ElementRef *m_ref;
  };

public:
  operator ConstElementRef() const noexcept;
  bool Exists() const noexcept;
  ElementRef operator[](const std::string &key) noexcept;
  ElementRef operator[](size_t index) noexcept;
  template <typename S> bool SetIfExist(const S &val) noexcept(!IS_DEBUG) {
    if (Exists()) {
      *this = val;
      return true;
    }
    return false;
  }
  Ptr operator&() const noexcept(!IS_DEBUG);
  template <typename T> operator T &() const noexcept(!IS_DEBUG) {
    static_assert(ReverseMap<std::remove_const_t<T>>::valid,
                  "Unsupported SysType used in conversion");
    return *reinterpret_cast<T *>(m_pByte + m_offset + m_pLayout->Resolve<T>());
  }
  template <typename T> T &operator=(const T &rhs) const noexcept(!IS_DEBUG) {
    static_assert(ReverseMap<std::remove_const_t<T>>::valid,
                  "Unsupported SysType used in conversion");
    return static_cast<T &>(*this) = rhs;
  }

private:
  ElementRef(size_t offsett, char *pByte,
             const LayoutElement *playout) noexcept;
  size_t m_offset;
  const LayoutElement *m_pLayout;
  char *m_pByte;
};

class Buffer {
public:
  Buffer() = default;
  Buffer(RawLayout &&layout) noexcept;
  Buffer(const CookedLayout &layout) noexcept;
  Buffer(CookedLayout &&layout) noexcept;
  Buffer(const Buffer &buffer) noexcept(!IS_DEBUG);
  Buffer(Buffer &&buffer) noexcept(!IS_DEBUG);
  ElementRef operator[](const std::string &key) noexcept(!IS_DEBUG);
  ConstElementRef operator[](const std::string &key) const noexcept(!IS_DEBUG);
  Buffer &operator=(const Buffer &right) noexcept(!IS_DEBUG);
  size_t GetSizeInBytes() const noexcept;
  const char *GetData() const noexcept;
  const LayoutElement &GetRootLayoutElement() const noexcept;
  void CopyFrom(const Buffer &buffer) noexcept(!IS_DEBUG);
  std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;

private:
  std::shared_ptr<LayoutElement> m_pLayoutRoot;
  std::vector<char> m_Data;
};
}; // namespace DCBuf

#ifndef DCB_IMPL_SOURCE
#undef LEAF_ELEMENT_TYPES
#endif