#pragma once
#include "CameraIndicator.h"
#include "FOVIndicator.h"
#include <DirectXMath.h>
#include <string>
namespace Rgph {
class RenderGraph;
}
class Camera {
public:
  Camera(class Graphics &gfx, std::string szName, bool tethered = false);
  DirectX::XMMATRIX GetCameraMatrix() const;
  DirectX::XMMATRIX GetPerspectiveViewMX() const noexcept(!IS_DEBUG);
  void BindtoGFX(Graphics &gfx) const noexcept(!IS_DEBUG);
  const std::string &GetName() const noexcept;
  void SetTetheredState(bool state) noexcept;
  bool GetTetherdState() const noexcept;
  void SetDeleteState(bool should_delete) noexcept;
  bool ShouldDelete() const noexcept;
  DirectX::XMFLOAT3 GetPos() const noexcept;
  void SetPos(float x, float y, float z);
  void SetPos(DirectX::XMFLOAT3 pos);
  void Rotate(float dx, float dy) noexcept;
  void Translate(float dx, float dy, float dz);
  void DisableIndicator() noexcept;
  bool MouseStatus() const noexcept;
  void UpdateDefaultValues(class Graphics &gfx) noexcept;
  void ShowControlCameraWidget() noexcept(!IS_DEBUG);
  void ShowActiveCameraWidget() noexcept(!IS_DEBUG);
  void HideMouse() noexcept;
  void ShowMouse() noexcept;
  void LinkTechniques(Rgph::RenderGraph &rg);
  void Submit(size_t channel) const;
  void ResetProjection() noexcept;

private:
  void Reset() noexcept;
  DirectX::XMMATRIX GetFPMatrix() const;
  DirectX::XMMATRIX GetTPMatrix() const;

private:
  Graphics &m_gfx;
  std::string m_szName;
  bool m_should_delete = false;
  bool m_hideMouse = false;
  bool m_tether_state = false;
  unsigned int m_defaultViewWidth;
  unsigned int m_defaultViewHeight;
  float m_viewWidth;
  float m_viewHeight;
  float m_NearZ;
  float m_FarZ;
  FOVIndicator m_fov_indi;
  CameraIndicator m_indicator;
  DirectX::XMFLOAT3 m_pos;
  DirectX::XMFLOAT3 m_rot;
  bool m_enCamIndi = true;
  bool m_enFOVIndi = true;
  static constexpr float m_rot_speed = 0.004f;
  static constexpr DirectX::XMFLOAT3 homePos = {0.0f, 100.0f, -25.0f};
};
