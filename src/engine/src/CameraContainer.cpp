#include "CameraContainer.h"
#include "Signal.h"
#include <imgui.h>
CameraContainer::CameraContainer(Graphics &gfx) : m_gfx(gfx) {
  for (size_t i = 0; i < 2; i++) {
    Add(gfx);
  }
}
Camera &CameraContainer::GetCamera() noexcept {
  return *m_Container[m_active_cam_idx];
}

void CameraContainer::Bind(Graphics &gfx) noexcept(!IS_DEBUG) {
#define SIGNAL_FUNCTION                                                        \
  for (auto &i : m_Container) {                                                \
    i->UpdateDefaultValues(gfx);                                               \
    i->ResetProjection();                                                      \
  }
  SIGNAL(gfx.sizeSignalPrj, SIGNAL_FUNCTION);
#undef SIGNAL_FUNCTION
  m_Container[m_active_cam_idx]->BindtoGFX(gfx);
}

void CameraContainer::SpawControlWindow() noexcept(!IS_DEBUG) {
  if (ImGui::Begin("Cameras")) {
    ImGui::Text("Active Camera");
    if (ImGui::BeginCombo("Current Camera",
                          m_Container[m_active_cam_idx]->GetName().c_str())) {
      for (int n = 0; n < m_Container.size(); n++) {
        const bool isSelected = m_active_cam_idx == n;
        if (ImGui::Selectable(m_Container[n]->GetName().c_str(), isSelected)) {
          m_active_cam_idx = n;
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    if (ImGui::Button("Add Camera")) {
      Add(m_gfx);
      signalCamAdded = true;
    }
    if (m_Container.size() > 1) {
      if (!m_Container[m_active_cam_idx]->GetTetherdState()) {
        if (ImGui::Button("Delete Current Camera")) {
          DeleteCurCamera();
        }
      } else {
        ImGui::Text("This camera is tethered to other object");
      }
    }
    m_Container[m_active_cam_idx]->ShowActiveCameraWidget();
    ImGui::Text("Controled Camera");
    if (ImGui::BeginCombo("Controled Camera",
                          m_Container[m_selected_cam_idx]->GetName().c_str())) {
      for (int n = 0; n < m_Container.size(); n++) {
        const bool isSelected = m_selected_cam_idx == n;
        if (ImGui::Selectable(m_Container[n]->GetName().c_str(), isSelected)) {
          m_selected_cam_idx = n;
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    m_Container[m_selected_cam_idx]->ShowControlCameraWidget();
  }
  ImGui::End();
}

void CameraContainer::DeleteCurCamera() noexcept(!IS_DEBUG) {
  m_Container.erase(m_Container.begin() + m_active_cam_idx);
  m_active_cam_idx == 0 ? m_active_cam_idx = 0 : m_active_cam_idx--;
}

void CameraContainer::Add(std::shared_ptr<Camera> camera) noexcept(!IS_DEBUG) {
  m_Container.push_back(std::move(camera));
  m_lifeTimeSize++;
}

void CameraContainer::Add(Graphics &gfx) noexcept(!IS_DEBUG) {
  m_Container.emplace_back(std::make_shared<Camera>(
      gfx, "Camera" + std::to_string(m_lifeTimeSize++)));
}

void CameraContainer::LinkTechniques(Rgph::RenderGraph &rg) {
  for (auto &i : m_Container) {
    i->LinkTechniques(rg);
  }
}

void CameraContainer::LinkAddedCamera(Rgph::RenderGraph &rg) {
  (*(m_Container.end() - 1))->LinkTechniques(rg);
}

void CameraContainer::Submit(size_t channel) const {
  for (size_t i = 0; i < m_Container.size(); i++) {
    if (i != m_active_cam_idx)
      m_Container[i]->Submit(channel);
  }
}
