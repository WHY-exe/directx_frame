#include "VertexBuffer.h"
#include "BindableCodex.h"
VertexBuffer::VertexBuffer(Graphics &gfx, const Vertex::DataBuffer &vbuf)
    : VertexBuffer(gfx, L"?", vbuf) {}
VertexBuffer::VertexBuffer(Graphics &gfx, const std::wstring &tag,
                           const Vertex::DataBuffer &vbuf)
    : m_uStride((UINT)vbuf.GetLayout().Size()), m_Tag(tag) {
  IMPORT_INFOMAN(gfx);
  // buffer description
  D3D11_BUFFER_DESC vbbd = {};
  vbbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vbbd.Usage = D3D11_USAGE_DEFAULT;
  vbbd.CPUAccessFlags = 0u;
  vbbd.MiscFlags = 0u;
  vbbd.ByteWidth = (UINT)vbuf.ByteWidth();
  vbbd.StructureByteStride = m_uStride;
  D3D11_SUBRESOURCE_DATA sdVerts = {};
  sdVerts.pSysMem = vbuf.GetData();
  GFX_THROW_INFO_ONLY(
      GetDevice(gfx)->CreateBuffer(&vbbd, &sdVerts, &m_pBuffer));
}

std::shared_ptr<VertexBuffer>
VertexBuffer::Resolve(Graphics &gfx, const std::wstring &tag,
                      const Vertex::DataBuffer buffer) noexcept {
  return CodeX::Resolve<VertexBuffer>(gfx, tag, buffer);
}

std::wstring VertexBuffer::GenUID_(std::wstring tag) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(typeid(VertexBuffer).name()) + L"#" + tag;
}

std::wstring VertexBuffer::GetUID() const noexcept { return GenUID(m_Tag); }

void VertexBuffer::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  // bind the vertex buffer to the pipeline
  const UINT vboffset = 0u;
  GetContext(gfx)->IASetVertexBuffers(0u, 1u, m_pBuffer.GetAddressOf(),
                                      &m_uStride, &vboffset);
}
