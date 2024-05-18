#pragma once
#include "Camera.h"
#include "Graphics.h"
#include <memory>
#include <vector>
class CameraContainer {
public:
  CameraContainer(Graphics &gfx);
  Camera &GetCamera() noexcept;
  void Bind(Graphics &gfx) noexcept(!IS_DEBUG);
  void SpawControlWindow() noexcept(!IS_DEBUG);
  void DeleteCurCamera() noexcept(!IS_DEBUG);
  void Add(std::shared_ptr<Camera> camera) noexcept(!IS_DEBUG);
  void Add(Graphics &gfx) noexcept(!IS_DEBUG);
  void LinkTechniques(Rgph::RenderGraph &rg);
  void LinkAddedCamera(Rgph::RenderGraph &rg);
  void Submit(size_t channel) const;
  bool signalCamAdded = false;

private:
  unsigned int m_active_cam_idx = 0u;
  unsigned int m_selected_cam_idx = 0u;
  unsigned int m_lifeTimeSize = 1u;
  Graphics &m_gfx;
  std::vector<std::shared_ptr<Camera>> m_Container;
};
