#pragma once
#include <Windows.h>

struct Menus
{
  static void InitializeClientMenu(HINSTANCE);
  static void InitializeSettingsMenu(HINSTANCE);
  static void UpdateMoveSpeedBar(int);
  static void UpdateMouseSpeedBar(int);
  static void UpdateFOVBar(int);
  static void UpdateRenderDistBar(int);
  static float GetMoveSpeed();
  static float GetMouseSpeed();
  static float GetFOV();  
  static float GetRenderDist();
  static void SetMaxSamples(unsigned int);
  static void SetWindowedMode();
  static void SetBorderlessMode();
  static void SetFullScreenMode();
};