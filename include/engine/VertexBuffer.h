#pragma once
#include "Bindable.h"
#include "GfxThrowMacro.h"
#include "StrManager.h"
#include "Vertex.h"
#include <memory>
#include <typeinfo>
class VertexBuffer : public Bindable {
public:
  template <class V>
  VertexBuffer(Graphics &gfx, const std::vector<V> &vertices)
      : m_uStride(sizeof(V)) {
    IMPORT_INFOMAN(gfx);
    // buffer description
    D3D11_BUFFER_DESC vbbd = {};
    vbbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbbd.Usage = D3D11_USAGE_DEFAULT;
    vbbd.CPUAccessFlags = 0u;
    vbbd.MiscFlags = 0u;
    vbbd.ByteWidth = (UINT)vertices.size() * m_uStride;
    vbbd.StructureByteStride = m_uStride;
    D3D11_SUBRESOURCE_DATA sdVerts = {};
    sdVerts.pSysMem = vertices.data();
    GFX_THROW_INFO_ONLY(
        GetDevice(gfx)->CreateBuffer(&vbbd, &sdVerts, &m_pBuffer));
  }
  VertexBuffer(Graphics &gfx, const Vertex::DataBuffer &buffer);
  VertexBuffer(Graphics &gfx, const std::wstring &tag,
               const Vertex::DataBuffer &buffer);
  static std::shared_ptr<VertexBuffer>
  Resolve(Graphics &gfx, const std::wstring &tag,
          const Vertex::DataBuffer buffer) noexcept;
  template <typename... Ignore>
  static std::wstring GenUID(std::wstring tag, Ignore &&...) noexcept {
    return GenUID_(tag);
  }
  std::wstring GetUID() const noexcept override;
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override;

private:
  static std::wstring GenUID_(std::wstring tag) noexcept;

private:
  std::wstring m_Tag;
  UINT m_uStride;
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
};
