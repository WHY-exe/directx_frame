#pragma once
#include "Bindable.h"
#include "BindableCodex.h"
#include "GfxThrowMacro.h"
template <typename C> class ConstantBuffer : public Bindable {
public:
  void Update(Graphics &gfx, const C &consts) {
    IMPORT_INFOMAN(gfx);
    // the pointer point to the memory of data that will be updated
    D3D11_MAPPED_SUBRESOURCE msr;
    // lock the memory and give the msr the pointer
    // ���������������뱣֤�󶨵����ݴ�СΪ16�ֽڵı���������ͨ����padding��ʵ��
    GFX_THROW_INFO(GetContext(gfx)->Map(m_pBuffer.Get(), 0u,
                                        D3D11_MAP_WRITE_DISCARD, 0u, &msr));
    memcpy(msr.pData, &consts, sizeof(consts));
    GetContext(gfx)->Unmap(m_pBuffer.Get(), 0u);
  }
  ConstantBuffer(Graphics &gfx, const C &consts, UINT slot = 0u)
      : m_slot(slot) {
    IMPORT_INFOMAN(gfx);
    D3D11_BUFFER_DESC cbbd = {};
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.Usage = D3D11_USAGE_DYNAMIC;
    cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbbd.MiscFlags = 0u;
    cbbd.ByteWidth = sizeof(C);
    cbbd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA sdcb = {};
    sdcb.pSysMem = &consts;
    GFX_THROW_INFO_ONLY(GetDevice(gfx)->CreateBuffer(&cbbd, &sdcb, &m_pBuffer));
  }
  ConstantBuffer(Graphics &gfx, UINT slot = 0u) : m_slot(slot) {
    IMPORT_INFOMAN(gfx);
    D3D11_BUFFER_DESC cbbd = {};
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.Usage = D3D11_USAGE_DYNAMIC;
    cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbbd.MiscFlags = 0u;
    cbbd.ByteWidth = sizeof(C);
    cbbd.StructureByteStride = 0u;
    GFX_THROW_INFO_ONLY(
        GetDevice(gfx)->CreateBuffer(&cbbd, nullptr, &m_pBuffer));
  }

protected:
  Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
  UINT m_slot;
};

template <typename C> class VertexConstantBuffer : public ConstantBuffer<C> {
  using ConstantBuffer<C>::m_pBuffer;
  using ConstantBuffer<C>::m_slot;
  using Bindable::GetContext;

public:
  using ConstantBuffer<C>::ConstantBuffer;

  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override {
    GetContext(gfx)->VSSetConstantBuffers(m_slot, 1u, m_pBuffer.GetAddressOf());
  }
  static std::shared_ptr<Bindable> Resolve(Graphics &gfx, const C &consts,
                                           UINT slot = 0) {
    return CodeX::Resolve<VertexConstantBuffer>(gfx, consts, slot);
  }
  static std::wstring GenUID(const C &consts, UINT slot = 0) {
    using namespace std::string_literals;
    return ANSI_TO_UTF8_STR(typeid(VertexConstantBuffer).name() + "#"s +
                            std::to_string(slot));
  }
  static std::wstring GenUID(UINT slot = 0) {
    using namespace std::string_literals;
    return ANSI_TO_UTF8_STR(typeid(VertexConstantBuffer).name() + "#"s +
                            std::to_string(slot));
  }
  std::wstring GetUID() const noexcept override { return GenUID(m_slot); }
};
template <typename C> class PixelConstantBuffer : public ConstantBuffer<C> {
  using ConstantBuffer<C>::m_pBuffer;
  using ConstantBuffer<C>::m_slot;
  using Bindable::GetContext;

public:
  using ConstantBuffer<C>::ConstantBuffer;

  void Bind(Graphics &gfx) noexcept(!IS_DEBUG) override {
    GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_pBuffer.GetAddressOf());
  }
  static std::shared_ptr<Bindable> Resolve(Graphics &gfx, const C &consts,
                                           UINT slot = 0) {
    return CodeX::Resolve<PixelConstantBuffer>(gfx, consts, slot);
  }
  static std::wstring GenUID(const C &consts, UINT slot = 0) {
    using namespace std::string_literals;
    return ANSI_TO_UTF8_STR(typeid(PixelConstantBuffer).name() + "#"s +
                            std::to_string(slot));
  }
  static std::wstring GenUID(UINT slot = 0) {
    using namespace std::string_literals;
    return ANSI_TO_UTF8_STR(typeid(PixelConstantBuffer).name() + "#"s +
                            std::to_string(slot));
  }
  std::wstring GetUID() const noexcept override { return GenUID(m_slot); }
};