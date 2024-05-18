#include "LightContainer.h"
#include "Signal.h"
#include "imgui.h"
LightContainer::LightContainer(Graphics &gfx) : m_gfx(gfx) {
  DCBuf::RawLayout cBufferLayout;
  cBufferLayout.Add<DCBuf::Integer>("cur_light_num");
  for (auto &i : m_ConstantBufferElements) {
    cBufferLayout.Add<DCBuf::Array>(i);
    if (i == "lightPositions" || i == "lightAmbients" || i == "lightDiffuses")
      cBufferLayout[i].Set<DCBuf::Float3>(MAX_LIGHT_NUM);
    else if (i == "lightIntensities" || i == "attConst" || i == "attLinear" ||
             i == "attQuad")
      cBufferLayout[i].Set<DCBuf::Float>(MAX_LIGHT_NUM);
    else if (i == "Enable")
      cBufferLayout[i].Set<DCBuf::Bool>(MAX_LIGHT_NUM);
  }

  DCBuf::Buffer CBuffer = DCBuf::Buffer(std::move(cBufferLayout));
  CBuffer["cur_light_num"] = (int)(m_point_lights.size());
  for (size_t i = 0; i < m_point_lights.size(); i++) {
    UpdateCBuffer(i, CBuffer);
  }

  m_PL_PSCbuf = std::make_unique<CachingPixelConstantBuffer>(gfx, CBuffer, 0u);
  // TestCode: add two lights
  for (size_t i = 0; i < 2; i++) {
    AddPointLight(gfx);
  }
  DISABLE_SIGNAL(lightAddedSignal);
}

void LightContainer::Bind(Graphics &gfx) noexcept {
  DCBuf::Buffer CBuffer = m_PL_PSCbuf->GetBuffer();
  for (size_t i = 0; i < m_point_lights.size(); i++) {
    m_point_lights[i]->Update();
    UpdateCBuffer(i, CBuffer);
  }
  CBuffer["cur_light_num"] = (int)(m_point_lights.size());
  m_PL_PSCbuf->SetBuffer(CBuffer);
  m_PL_PSCbuf->Bind(gfx);
}

void LightContainer::Submit(size_t channel) noexcept {
  for (auto &i : m_point_lights) {
    i->Submit(channel);
  }
}

const std::unique_ptr<PointLight> &LightContainer::GetBack() const noexcept {
  return m_point_lights.back();
}

void LightContainer::LinkAddedLight(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  m_point_lights.back()->LinkTechniques(rg);
}

void LightContainer::LinkTechniques(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  for (auto &i : m_point_lights) {
    i->LinkTechniques(rg);
  }
}

void LightContainer::AddPointLight(Graphics &gfx) noexcept(!IS_DEBUG) {
  m_point_lights.emplace_back(
      std::make_unique<PointLight>(gfx, m_life_time_light_num));
  lightAddedSignal = true;
  m_life_time_light_num++;
}

void LightContainer::DeleteCurLight() noexcept {
  m_point_lights.erase(m_point_lights.begin() + m_curPL_Index);
  m_curPL_Index == 0 ? m_curPL_Index = 0 : m_curPL_Index--;
  for (size_t i = 0; i < m_point_lights.size(); i++) {
    m_point_lights[i]->m_index = (int)i;
  }
}

void LightContainer::SpwanControlWindow() noexcept(!IS_DEBUG) {
  if (ImGui::Begin("Lights")) {
    ImGui::Text("Active Light");
    if (ImGui::BeginCombo("Current Light",
                          m_point_lights[m_curPL_Index]->GetName().c_str())) {
      for (int n = 0; n < m_point_lights.size(); n++) {
        const bool isSelected = m_curPL_Index == n;
        if (ImGui::Selectable(m_point_lights[n]->GetName().c_str(),
                              isSelected)) {
          m_curPL_Index = n;
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    if (m_point_lights.size() < MAX_LIGHT_NUM) {
      if (ImGui::Button("Add Light")) {
        AddPointLight(m_gfx);
      }
    } else {
      ImGui::Text("Has reached the maximum light number the system support");
    }
    if (m_point_lights.size() > 1) {
      if (ImGui::Button("Delete Current Light")) {
        DeleteCurLight();
      }
    }
    m_point_lights[m_curPL_Index]->SpwanControlWidgets();
  }
  ImGui::End();
}

const std::vector<std::unique_ptr<PointLight>> &
LightContainer::GetContainer() const noexcept {
  return m_point_lights;
}

void LightContainer::UpdateCBuffer(size_t index,
                                   DCBuf::Buffer &cbuffer) noexcept(!IS_DEBUG) {
  const auto worPos = DirectX::XMLoadFloat3(&m_point_lights[index]->m_pos);
  DirectX::XMFLOAT3 ViewPos;
  DirectX::XMStoreFloat3(
      &ViewPos, DirectX::XMVector3Transform(worPos, m_gfx.GetCamera()));
  cbuffer["lightPositions"][index] = ViewPos;
  cbuffer["lightAmbients"][index] = m_point_lights[index]->m_ambient;
  cbuffer["lightDiffuses"][index] = m_point_lights[index]->m_diffuseColor;
  cbuffer["lightIntensities"][index] =
      m_point_lights[index]->m_diffuseIntensity;
  cbuffer["attConst"][index] = m_point_lights[index]->m_attConst;
  cbuffer["attLinear"][index] = m_point_lights[index]->m_attLinear;
  cbuffer["attQuad"][index] = m_point_lights[index]->m_attQuad;
  cbuffer["Enable"][index] = m_point_lights[index]->m_Enable;
}

std::vector<std::string> LightContainer::m_ConstantBufferElements = {
    "lightPositions", "lightAmbients", "lightDiffuses", "lightIntensities",
    "attConst",       "attLinear",     "attQuad",       "Enable"};