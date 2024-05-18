#pragma once
#include "Bindable.h"
#include "DynamicConstantBuffer.h"
#include "GfxThrowMacro.h"
class ConstantBufferEx : public Bindable {
public:
  void Update(Graphics &gfx, const DCBuf::Buffer &buffer) {
    IMPORT_INFOMAN(gfx);
    // the pointer point to the memory of data that will be updated
    D3D11_MAPPED_SUBRESOURCE msr;
    // lock the memory and give the msr the pointer
    GFX_THROW_INFO(GetContext(gfx)->Map(m_pBuffer.Get(), 0u,
                                        D3D11_MAP_WRITE_DISCARD, 0u, &msr));
    memcpy(msr.pData, buffer.GetData(), buffer.GetSizeInBytes());
    GetContext(gfx)->Unmap(m_pBuffer.Get(), 0u);
  }

protected:
  ConstantBufferEx(Graphics &gfx, const DCBuf::Buffer *pBuffer,
                   const DCBuf::LayoutElement &layoutRoot, UINT slot)
      : m_slot(slot) {
    IMPORT_INFOMAN(gfx);
    D3D11_BUFFER_DESC cbbd = {};
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.Usage = D3D11_USAGE_DYNAMIC;
    cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbbd.MiscFlags = 0u;
    cbbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
    cbbd.StructureByteStride = 0u;
    if (pBuffer != nullptr) {
      D3D11_SUBRESOURCE_DATA csd = {};
      csd.pSysMem = pBuffer->GetData();
      GFX_THROW_INFO_ONLY(
          GetDevice(gfx)->CreateBuffer(&cbbd, &csd, &m_pBuffer));
    } else {
      GFX_THROW_INFO_ONLY(
          GetDevice(gfx)->CreateBuffer(&cbbd, nullptr, &m_pBuffer));
    }
  }

protected:
  UINT m_slot;
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
};

class PixelConstantBufferEx : public ConstantBufferEx {
public:
  using ConstantBufferEx::ConstantBufferEx;
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override {
    GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_pBuffer.GetAddressOf());
  }
};
class VertexConstantBufferEx : public ConstantBufferEx {
public:
  using ConstantBufferEx::ConstantBufferEx;
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override {
    GetContext(gfx)->VSSetConstantBuffers(m_slot, 1u, m_pBuffer.GetAddressOf());
  }
};

template <class T> class CachingConstantBuffer : public T {
public:
  CachingConstantBuffer(Graphics &gfx, const DCBuf::CookedLayout &layoutRoot,
                        UINT slot)
      : T(gfx, nullptr, *layoutRoot.ShareRoot(), slot), m_Buffer(layoutRoot) {
    m_dirty = true;
  }
  CachingConstantBuffer(Graphics &gfx, const DCBuf::Buffer &buffer, UINT slot)
      : T(gfx, &buffer, buffer.GetRootLayoutElement(), slot), m_Buffer(buffer) {
  }
  const DCBuf::LayoutElement &GetRootElement() const noexcept {
    return m_Buffer.GetRootLayoutElement();
  }
  const DCBuf::Buffer &GetBuffer() const noexcept { return m_Buffer; }
  DCBuf::Buffer &GetBuffer() noexcept { return m_Buffer; }
  void SetBuffer(const DCBuf::Buffer &src_buffer) noexcept {
    m_Buffer.CopyFrom(src_buffer);
    m_dirty = true;
  }
  void Accept(Probe &probe) noexcept override {
    if (probe.VisitBuffer(m_Buffer)) {
      m_dirty = true;
    }
  }
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override {
    if (m_dirty) {
      T::Update(gfx, m_Buffer);
      m_dirty = false;
    }
    T::Bind(gfx);
  }

private:
  DCBuf::Buffer m_Buffer;
  bool m_dirty = false;
};

using CachingPixelConstantBuffer = CachingConstantBuffer<PixelConstantBufferEx>;
using CachingVertexConstantBuffer =
    CachingConstantBuffer<VertexConstantBufferEx>;