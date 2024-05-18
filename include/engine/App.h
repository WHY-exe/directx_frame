#pragma once
#include "BlurOutlineRenderGraph.h"
#include "CameraContainer.h"
#include "Drawable.h"
#include "ImguiManager.h"
#include "LightContainer.h"
#include "Scene.h"
#include "SceneProbe.h"
#include "Timer.h"
#include "Window.h"
class App {
public:
  App();
  WPARAM Run();

private:
  void DoFrame();
  void DoWinLogic();

private:
  int x = 0, y = 0;
  ImguiManager m_imguiMan;
  Window m_wnd;
  Graphics &m_gfx;
  Timer timer;

private:
  // user data
  double m_delta_time = 0.0;
  Rgph::BlurOutlineRenderGraph m_rg;
  Scene::Scene scene;
  Scene::ModelProbe probe;
  CameraContainer cams;
  LightContainer lights;
  // GlobalLight gLight;
};
