#include "Light.h"
#include "imgui.h"
Light::Light(Graphics &gfx, std::string szName, int index) noexcept
    : m_szName(szName), m_index(index), m_pos(0.0f, 0.0f, 0.0f) {
  m_pCamera =
      std::make_shared<Camera>(gfx, "lightCam" + std::to_string(index), true);
}

const std::string &Light::GetName() const noexcept { return m_szName; }

void Light::SpwanControlWidgets() noexcept {
  ImGui::Text("Enable");
  ImGui::Checkbox("Enable", reinterpret_cast<bool *>(&m_Enable));
  ImGui::Text("Position");
  ImGui::SliderFloat("X", &m_pos.x, -2000.0f, 2000.0f, "%.1f");
  ImGui::SliderFloat("Y", &m_pos.y, -2000.0f, 2000.0f, "%.1f");
  ImGui::SliderFloat("Z", &m_pos.z, -2000.0f, 2000.0f, "%.1f");
  ImGui::ColorEdit3("DiffuseColor", &m_diffuseColor.x);
  ImGui::ColorEdit3("AmbientColor", &m_ambient.x);
  ImGui::Text("LightIntensity");
  ImGui::SliderFloat("Intensity", &m_diffuseIntensity, 0.0f, 10.0f, "%.2f");
}

std::shared_ptr<Camera> Light::ShareCamera() const noexcept {
  return m_pCamera;
}

PointLight::PointLight(Graphics &gfx, int index)
    : Light(gfx, "PointLight" + std::to_string(index), index),
      m_indicator(gfx) {
  m_pos = {0.0f, 500.0f, 0.0f};
  m_indicator.SetScale(10.0f);
  m_pCamera->DisableIndicator();
}

void PointLight::Update() noexcept {
  m_indicator.SetPos(m_pos);
  m_pCamera->SetPos(m_pos);
}

void PointLight::Submit(size_t channel) noexcept {
  if (m_Enable && m_showIndicator)
    m_indicator.Submit(channel);
}

void PointLight::LinkTechniques(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  m_indicator.LinkTechniques(rg);
}

void PointLight::SpwanControlWidgets() noexcept {
  Light::SpwanControlWidgets();
  ImGui::Checkbox("show indicator", &m_showIndicator);
  ImGui::Text("Attenuation");
  ImGui::SliderFloat("AttConst", &m_attConst, 0.0f, 0.1f, "%.5f");
  ImGui::SliderFloat("AttLinear", &m_attLinear, 0.0f, 0.1f, "%.5f");
  ImGui::SliderFloat("AttQuad", &m_attQuad, 0.0f, 0.1f, "%.5f");
}
