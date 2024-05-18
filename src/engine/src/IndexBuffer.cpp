#include "IndexBuffer.h"
#include "BindableCodex.h"
#include "StrManager.h"
IndexBuffer::IndexBuffer(Graphics &gfx, const std::vector<UINT> &indicies)
    : IndexBuffer(gfx, L"?", indicies) {}

IndexBuffer::IndexBuffer(Graphics &gfx, const std::wstring &tag,
                         const std::vector<UINT> &indicies)
    : m_Tag(tag), m_uBufferSize((UINT)indicies.size()) {
  IMPORT_INFOMAN(gfx);
  D3D11_BUFFER_DESC ibbd = {};
  ibbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibbd.Usage = D3D11_USAGE_DEFAULT;
  ibbd.CPUAccessFlags = 0u;
  ibbd.ByteWidth = (UINT)indicies.size() * sizeof(UINT);
  ibbd.StructureByteStride = sizeof(UINT);
  D3D11_SUBRESOURCE_DATA sdIdics = {};
  sdIdics.pSysMem = indicies.data();

  GFX_THROW_INFO_ONLY(
      GetDevice(gfx)->CreateBuffer(&ibbd, &sdIdics, &m_IndexBuffer));
}

std::shared_ptr<IndexBuffer>
IndexBuffer::Resolve(Graphics &gfx, const std::wstring &tag,
                     const std::vector<UINT> &indicies) noexcept {
  return CodeX::Resolve<IndexBuffer>(gfx, tag, indicies);
}

std::wstring IndexBuffer::GetUID() const noexcept { return GenUID(m_Tag); }

void IndexBuffer::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  GetContext(gfx)->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT,
                                    0u);
}

UINT IndexBuffer::GetSize() const { return m_uBufferSize; }

std::wstring IndexBuffer::GenUID_(std::wstring tag) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(typeid(IndexBuffer).name()) + L"#" + tag;
}
