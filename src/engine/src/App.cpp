#include "App.h"
#include "Channel.h"
#include "Signal.h"

App::App()
    : m_imguiMan(), m_wnd(L"Engine", 1000, 700), m_gfx(m_wnd.GetGfx()),
      // gLight(m_gfx),
      m_rg(m_gfx), cams(m_gfx), lights(m_gfx) {
  Scene::ModelSetting op1;
  op1.szModelPath = "res\\Models\\Sponza\\sponza.obj";
  op1.szModelName = "sponza";
  scene.AddModel(m_gfx, op1);
  DISABLE_SIGNAL(scene.signalModelAdded);
  Scene::ModelSetting op2;
  op2.szModelPath = "res\\Models\\nanosuit.obj";
  op2.szModelName = "nano";
  scene.AddModel(m_gfx, op2);
  DISABLE_SIGNAL(scene.signalModelAdded);
  scene.LinkTechniques(m_rg);
  cams.LinkTechniques(m_rg);
  lights.LinkTechniques(m_rg);
  m_rg.BindShadowCamera(lights);
}

WPARAM App::Run() {
  while (true) {
    timer.Mark();
    // c++ 17 required
    if (const auto ecode = Window::RunWindow()) {
      return *ecode;
    }
    if (m_delta_time >= (1.0f / 30.0f)) {
      DoWinLogic();
      DoFrame();
      m_delta_time = 0.0;
    }
    m_delta_time += timer.Peek();
  }
}

void App::DoFrame() {
  m_gfx.BeginFrame();

  SIGNAL(cams.signalCamAdded, cams.LinkAddedCamera(m_rg));

  SIGNAL(scene.signalModelAdded, scene.LinkAddedModel(m_rg));
#define SIGNAL_FUNTION                                                         \
  lights.LinkAddedLight(m_rg);                                                 \
  cams.Add(lights.GetBack()->ShareCamera());                                   \
  cams.LinkAddedCamera(m_rg);                                                  \
  DISABLE_SIGNAL(cams.signalCamAdded)
  SIGNAL(lights.lightAddedSignal, SIGNAL_FUNTION);
#undef SIGNAL_FUNTION
  m_rg.BindMainCamera(cams.GetCamera());
  scene.Submit(Channel::main | Channel::shadow);
  cams.Submit(Channel::main);
  lights.Submit(Channel::main);
  cams.Bind(m_gfx);
  lights.Bind(m_gfx);
  m_rg.Execute(m_gfx);

  probe.SpwanControlWindow(scene);
  cams.SpawControlWindow();
  lights.SpwanControlWindow();
  m_rg.RenderShadowWigets(m_gfx);
  //
  m_gfx.EndFrame();
  m_rg.Reset();
}

void App::DoWinLogic() {
  if (m_wnd.kbd.KeyIsPressed(VK_ESCAPE)) {
    cams.GetCamera().ShowMouse();
  }
  if (cams.GetCamera().MouseStatus()) {
    m_wnd.DisableCursor();
  } else
    m_wnd.EnableCursor();
  if (m_wnd.kbd.KeyIsPressed('W')) {
    cams.GetCamera().Translate(0.0f, 0.0f, 10.0f);
  }
  if (m_wnd.kbd.KeyIsPressed('A')) {
    cams.GetCamera().Translate(-10.0f, 0.0f, 0.0f);
  }
  if (m_wnd.kbd.KeyIsPressed('R')) {
    cams.GetCamera().Translate(0.0f, 10.0f, 0.0f);
  }
  if (m_wnd.kbd.KeyIsPressed('S')) {
    cams.GetCamera().Translate(0.0f, 0.0f, -10.0f);
  }
  if (m_wnd.kbd.KeyIsPressed('D')) {
    cams.GetCamera().Translate(10.0f, 0.0f, 0.0f);
  }
  if (m_wnd.kbd.KeyIsPressed('F')) {
    cams.GetCamera().Translate(0.0f, -10.0f, 0.0f);
  }

  while (auto d = m_wnd.mouse.ReadRawDelta()) {
    if (!m_wnd.CursorEnabled()) {
      cams.GetCamera().Rotate((float)d->x, (float)d->y);
    }
  }
}
