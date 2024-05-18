#include "Sampler.h"
#include "BindableCodex.h"
#include "GfxThrowMacro.h"
#include "StrManager.h"
#include <typeinfo>
Sampler::Sampler(Graphics &gfx, Type type, bool reflect, UINT slot)
    : m_type(type), m_reflect(reflect), m_slot(slot) {
  IMPORT_INFOMAN(gfx);
  D3D11_SAMPLER_DESC sampDesc = CD3D11_SAMPLER_DESC{CD3D11_DEFAULT{}};

  sampDesc.Filter = [type]() {
    switch (type) {
    case Sampler::Type::Anisotropic:
      return D3D11_FILTER_ANISOTROPIC;
      break;
    case Sampler::Type::Bilinear:
      return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
      break;
    default:
    case Sampler::Type::Point:
      return D3D11_FILTER_MIN_MAG_MIP_POINT;
      break;
    }
  }();
  sampDesc.AddressU =
      reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressV =
      reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW =
      reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

  GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&sampDesc, &m_pSampler));
}

void Sampler::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
  GetContext(gfx)->PSSetSamplers(m_slot, 1u, m_pSampler.GetAddressOf());
}

std::shared_ptr<Sampler> Sampler::Resolve(Graphics &gfx, Type type,
                                          bool reflect,
                                          UINT slot) noexcept(!IS_DEBUG) {
  return CodeX::Resolve<Sampler>(gfx, type, reflect);
}

std::wstring Sampler::GenUID(Type type, bool reflect) noexcept {
  using namespace std::string_literals;
  return ANSI_TO_UTF8_STR(typeid(Sampler).name() + std::to_string((int)type) +
                          (reflect ? "R"s : "W"s));
}

std::wstring Sampler::GetUID() const noexcept {
  return GenUID(m_type, m_reflect);
}
