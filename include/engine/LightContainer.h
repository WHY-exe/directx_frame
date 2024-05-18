#pragma once
#include "DynamicConstantBuffer.h"
#include "Light.h"
#include <memory>
#define MAX_LIGHT_NUM (unsigned int)16
class LightContainer {
public:
  LightContainer(Graphics &gfx);
  void Bind(Graphics &gfx) noexcept;
  void Submit(size_t channel) noexcept;
  const std::unique_ptr<PointLight> &GetBack() const noexcept;
  void LinkAddedLight(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG);
  void LinkTechniques(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG);
  void AddPointLight(Graphics &gfx) noexcept(!IS_DEBUG);
  void DeleteCurLight() noexcept;
  void SpwanControlWindow() noexcept(!IS_DEBUG);
  const std::vector<std::unique_ptr<PointLight>> &GetContainer() const noexcept;

public:
  bool lightAddedSignal = false;

private:
  void UpdateCBuffer(size_t index, DCBuf::Buffer &cbuffer) noexcept(!IS_DEBUG);

private:
  Graphics &m_gfx;
  static std::vector<std::string> m_ConstantBufferElements;
  // Point light managment
  unsigned int m_curPL_Index = 0u;
  unsigned int m_life_time_light_num = 1u;
  std::unique_ptr<CachingPixelConstantBuffer> m_PL_PSCbuf;
  std::vector<std::unique_ptr<PointLight>> m_point_lights;
};