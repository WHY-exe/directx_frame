#include "ImguiManager.h"

ImguiManager::ImguiManager() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  const auto io = ImGui::GetIO();
  ImFontConfig config;
  config.MergeMode = false;
  config.OversampleH = 2;
  config.OversampleV = 1;
  config.GlyphExtraSpacing.x = 1.0f;
  // io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf",
  //	14.0f, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
  io.Fonts->AddFontFromFileTTF("c:/windows/fonts/YuGothR.ttc", 14.0f, &config,
                               io.Fonts->GetGlyphRangesJapanese());
  io.Fonts->Build();
}

ImguiManager::~ImguiManager() { ImGui::DestroyContext(); }