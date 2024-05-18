#define DCB_IMPL_SOURCE
#include "DynamicConstantBuffer.h"
#include "LayoutCodeX.h"
#include <algorithm>
#include <vector>
namespace DCBuf {
struct ExtraData {
  struct Struct : public LayoutElement::ExtraDataBase {
    std::vector<std::pair<std::string, LayoutElement>> m_layoutElements;
  };
  struct Array : public LayoutElement::ExtraDataBase {
    std::optional<LayoutElement> m_layoutElement;
    size_t m_size;
  };
};

std::string LayoutElement::GetSignature() const noexcept(!IS_DEBUG) {
  switch (m_type) {
#define X(el)                                                                  \
  case el:                                                                     \
    return Map<el>::code;
    LEAF_ELEMENT_TYPES;
#undef X
  case DCBuf::Struct:
    return GetSignatureForStruct();
  case DCBuf::Array:
    return GetSignatureForArray();
  default:
    assert("Bad type in signature generation" && false);
    return "???";
  }
}
std::pair<size_t, const LayoutElement *>
LayoutElement::CaculateIndexingOffset(size_t index, size_t offset) const
    noexcept(!IS_DEBUG) {
  assert("Indexing into non-Array element" && m_type == Array);
  const auto &data = static_cast<ExtraData::Array &>(*m_pExtraData);
  assert(index < data.m_size);
  return {offset + data.m_size * index, &*data.m_layoutElement};
}
bool LayoutElement::Exists() const noexcept { return m_type != Empty; }
LayoutElement &
LayoutElement::operator[](const std::string &key) noexcept(!IS_DEBUG) {
  assert("key into non-Struct element" && m_type == Struct);
  for (auto &mem :
       static_cast<ExtraData::Struct &>(*m_pExtraData).m_layoutElements) {
    if (mem.first == key) {
      return mem.second;
    }
  }
  return GetEmptyElement();
}
const LayoutElement &LayoutElement::operator[](const std::string &key) const
    noexcept(!IS_DEBUG) {
  return const_cast<LayoutElement &>(*this)[key];
}
LayoutElement &LayoutElement::T() noexcept(!IS_DEBUG) {
  assert("accessing T of non-array" && m_type == Array);
  return *static_cast<ExtraData::Array &>(*m_pExtraData).m_layoutElement;
}
const LayoutElement &LayoutElement::T() const noexcept(!IS_DEBUG) {
  return const_cast<LayoutElement &>(*this).T();
}

size_t LayoutElement::GetOffsetBegin() const noexcept { return *m_offset; }

size_t LayoutElement::GetOffsetEnd() const noexcept {
  switch (m_type) {
#define X(el)                                                                  \
  case el:                                                                     \
    return *m_offset + Map<el>::hlsl_size;
    LEAF_ELEMENT_TYPES;
#undef X
  case Struct: {
    const auto &data = static_cast<ExtraData::Struct &>(*m_pExtraData);
    return AdvanceToBoundary(
        data.m_layoutElements.back().second.GetOffsetEnd());
  }
  case Array: {
    const auto &data = static_cast<ExtraData::Array &>(*m_pExtraData);
    return *m_offset +
           AdvanceToBoundary(data.m_layoutElement->GetSizeInBytes()) *
               data.m_size;
  }
  default:
    assert("Tried to get offset of empty or invalid element" && false);
    return 0u;
  }
}
LayoutElement &LayoutElement::Add(const std::string &name,
                                  DataType dt) noexcept(!IS_DEBUG) {
  assert("Add non-struct in layout" && m_type == Struct);
  assert("Invalid symbol name in Struct" && ValidateSymbolName(name));
  auto &struct_data =
      static_cast<ExtraData::Struct &>(*m_pExtraData).m_layoutElements;
  for (auto &mem : struct_data) {
    if (mem.first == name) {
      assert("Adding duplicate name to struct");
    }
  }
  struct_data.emplace_back(name, LayoutElement{dt});
  return *this;
}
LayoutElement &LayoutElement::Set(size_t size,
                                  DataType dt) noexcept(!IS_DEBUG) {
  assert("Setting non-Array in layout" && m_type == Array);
  assert(size != 0u);
  auto &array_data = static_cast<ExtraData::Array &>(*m_pExtraData);
  array_data.m_layoutElement = {dt};
  array_data.m_size = size;
  return *this;
}
size_t LayoutElement::GetSizeInBytes() const noexcept {
  return GetOffsetEnd() - GetOffsetBegin();
}
LayoutElement::LayoutElement(DataType dt) noexcept(!IS_DEBUG) : m_type(dt) {
  assert(dt != Empty);
  if (dt == Struct) {
    m_pExtraData = std::unique_ptr<ExtraData::Struct>{new ExtraData::Struct()};
  }
  if (dt == Array) {
    m_pExtraData = std::unique_ptr<ExtraData::Array>{new ExtraData::Array()};
  }
}

size_t LayoutElement::Finalize(size_t offsetIn) noexcept(!IS_DEBUG) {
  switch (m_type) {
#define X(el)                                                                  \
  case el:                                                                     \
    m_offset = AdvanceIfCrossBoundary(offsetIn, Map<el>::hlsl_size);           \
    return *m_offset + Map<el>::hlsl_size;
    LEAF_ELEMENT_TYPES
#undef X
  case Struct:
    return FinalizeForStruct(offsetIn);
  case Array:
    return FinalizeForArray(offsetIn);
  default:
    assert("Bad type in size computation" && false);
    return 0u;
  }
}
LayoutElement &LayoutElement::GetEmptyElement() noexcept {
  static LayoutElement result = {};
  return result;
}
std::string LayoutElement::GetSignatureForStruct() const noexcept {
  std::string result = "Str{";
  auto &data = static_cast<ExtraData::Struct &>(*m_pExtraData).m_layoutElements;
  for (auto &el : data) {
    result +=
        "Name : " + el.first + ", Signature" + el.second.GetSignature() + ";\n";
  }
  result += "}";
  return result;
}

size_t LayoutElement::FinalizeForStruct(size_t offsetIn) noexcept(!IS_DEBUG) {
  auto &data = static_cast<ExtraData::Struct &>(*m_pExtraData).m_layoutElements;
  m_offset = AdvanceToBoundary(offsetIn);
  auto offsetNext = *m_offset;
  for (auto &el : data) {
    offsetNext = el.second.Finalize(offsetNext);
  }
  return offsetNext;
}
size_t LayoutElement::FinalizeForArray(size_t offsetIn) noexcept(!IS_DEBUG) {
  auto &data = static_cast<ExtraData::Array &>(*m_pExtraData);
  m_offset = AdvanceToBoundary(offsetIn);
  data.m_layoutElement->Finalize(*m_offset);
  data.m_size =
      LayoutElement::AdvanceToBoundary(data.m_layoutElement->GetSizeInBytes());

  return GetOffsetEnd();
}
std::string LayoutElement::GetSignatureForArray() const noexcept {
  std::string result = "Arr:{";
  auto &data = static_cast<ExtraData::Array &>(*m_pExtraData);
  result += "Size : " + std::to_string(data.m_size) +
            ", DataType : " + data.m_layoutElement->GetSignature() + "};";
  return result;
}
// in gpu side's const buffer, data align in 16Byte
// this functoin allow us to advance to the  offset at the begining of of next
// block of data
size_t LayoutElement::AdvanceToBoundary(size_t offset) noexcept {
  return offset + (16u - offset % 16u) % 16u;
}

bool LayoutElement::CrossBoundary(size_t offset, size_t size) noexcept {
  auto pageStart = offset / 16u;
  auto pageEnd = (offset + size) / 16u;

  return (pageStart != pageEnd && (offset + size) % 16 != 0) || size > 16u;
}

size_t LayoutElement::AdvanceIfCrossBoundary(size_t offset,
                                             size_t size) noexcept {
  return CrossBoundary(offset, size) ? AdvanceToBoundary(offset) : offset;
}

bool LayoutElement::ValidateSymbolName(const std::string &name) noexcept {
  // symbols can contain alphanumeric and underscore, must not start with digit
  return !name.empty() && !std::isdigit(name.front()) &&
         std::all_of(name.begin(), name.end(),
                     [](char c) { return std::isalnum(c) || c == '_'; });
}

size_t Layout::GetSizeInBytes() const noexcept {
  return m_pRoot->GetSizeInBytes();
}
std::string Layout::GetSignature() const noexcept {
  return m_pRoot->GetSignature();
}
Layout::Layout(std::shared_ptr<LayoutElement> pRoot) noexcept
    : m_pRoot(std::move(pRoot)) {}

RawLayout::RawLayout() noexcept
    : Layout(std::shared_ptr<LayoutElement>{new LayoutElement{Struct}}) {}

LayoutElement &RawLayout::operator[](const std::string &key) noexcept {
  return (*m_pRoot)[key];
}
void RawLayout::ClearRoot() noexcept { *this = RawLayout(); }
std::shared_ptr<LayoutElement> RawLayout::DeliverRoot() noexcept {
  auto temp = std::move(m_pRoot);
  temp->Finalize(0);
  *this = RawLayout();
  return temp;
}

CookedLayout::CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept
    : Layout(std::move(pRoot)) {}
const LayoutElement &CookedLayout::operator[](const std::string &key) const
    noexcept(!IS_DEBUG) {
  return (*m_pRoot)[key];
}
std::shared_ptr<LayoutElement> CookedLayout::ShareRoot() const noexcept {
  return m_pRoot;
}
std::shared_ptr<LayoutElement> CookedLayout::RelinquishRoot() const noexcept {
  return std::move(m_pRoot);
}

bool ConstElementRef::Exists() const noexcept { return m_pLayout->Exists(); }
ConstElementRef ConstElementRef::operator[](const std::string &key) const
    noexcept(!IS_DEBUG) {
  return {m_offset, m_pByte, &(*m_pLayout)[key]};
}
ConstElementRef ConstElementRef::operator[](size_t index) const
    noexcept(!IS_DEBUG) {
  const auto data = m_pLayout->CaculateIndexingOffset(index, m_offset);
  return {data.first, m_pByte, data.second};
}
ConstElementRef::Ptr ConstElementRef::operator&() const noexcept(!IS_DEBUG) {
  return ConstElementRef::Ptr(this);
}
ConstElementRef::ConstElementRef(size_t offset, const char *pByte,
                                 const LayoutElement *playout)
    : m_offset(offset), m_pByte(pByte), m_pLayout(playout) {}
ConstElementRef::Ptr::Ptr(const ConstElementRef *ref) noexcept : m_ref(ref) {}

ElementRef::operator ConstElementRef() const noexcept {
  return {m_offset, m_pByte, m_pLayout};
}

bool ElementRef::Exists() const noexcept { return m_pLayout->Exists(); }
ElementRef::ElementRef(size_t offset, char *pByte,
                       const LayoutElement *playout) noexcept
    : m_offset(offset), m_pByte(pByte), m_pLayout(playout) {}
ElementRef ElementRef::operator[](const std::string &key) noexcept {
  return {m_offset, m_pByte, &(*m_pLayout)[key]};
}
ElementRef ElementRef::operator[](size_t index) noexcept {
  const auto data = m_pLayout->CaculateIndexingOffset(index, m_offset);
  return {data.first, m_pByte, data.second};
}
ElementRef::Ptr ElementRef::operator&() const noexcept(!IS_DEBUG) {
  return ElementRef::Ptr(const_cast<ElementRef *>(this));
}
ElementRef::Ptr::Ptr(ElementRef *ref) noexcept : m_ref(ref) {}

Buffer::Buffer(RawLayout &&layout) noexcept
    : Buffer(LayoutCodeX::Resolve(std::move(layout))) {}
Buffer::Buffer(const CookedLayout &layout) noexcept
    : m_pLayoutRoot(layout.ShareRoot()), m_Data(m_pLayoutRoot->GetOffsetEnd()) {
}
Buffer::Buffer(CookedLayout &&layout) noexcept
    : m_pLayoutRoot(layout.RelinquishRoot()),
      m_Data(m_pLayoutRoot->GetOffsetEnd()) {}
Buffer::Buffer(const Buffer &buffer) noexcept(!IS_DEBUG)
    : m_pLayoutRoot(buffer.m_pLayoutRoot), m_Data(buffer.m_Data) {}

Buffer::Buffer(Buffer &&buffer) noexcept(!IS_DEBUG)
    : m_pLayoutRoot(std::move(buffer.m_pLayoutRoot)),
      m_Data(std::move(buffer.m_Data)) {}

ElementRef Buffer::operator[](const std::string &key) noexcept(!IS_DEBUG) {
  return {0u, m_Data.data(), &(*m_pLayoutRoot)[key]};
}

ConstElementRef Buffer::operator[](const std::string &key) const
    noexcept(!IS_DEBUG) {
  return const_cast<Buffer &>(*this)[key];
}
Buffer &Buffer::operator=(const Buffer &right) noexcept(!IS_DEBUG) {
  m_pLayoutRoot = right.ShareLayoutRoot();
  m_Data = right.m_Data;
  return *this;
}
size_t Buffer::GetSizeInBytes() const noexcept { return m_Data.size(); }
const char *Buffer::GetData() const noexcept { return m_Data.data(); }

const LayoutElement &Buffer::GetRootLayoutElement() const noexcept {
  return const_cast<LayoutElement &>(*m_pLayoutRoot);
}

void Buffer::CopyFrom(const Buffer &buffer) noexcept(!IS_DEBUG) {
  assert(&GetRootLayoutElement() == &buffer.GetRootLayoutElement());
  std::copy(buffer.m_Data.begin(), buffer.m_Data.end(), m_Data.begin());
}

std::shared_ptr<LayoutElement> Buffer::ShareLayoutRoot() const noexcept {
  return m_pLayoutRoot;
}
} // namespace DCBuf
