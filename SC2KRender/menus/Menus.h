#pragma once
#include <Windows.h>

struct Menus
{
  static void InitializeClientMenu(HINSTANCE);
  static void InitializeSettingsMenu(HINSTANCE);
  static void UpdateMoveSpeedBar(int);
  static void UpdateMouseSpeedBar(int);
  static void UpdateZoomBar(int);
  static float GetMoveSpeed();
  static float GetMouseSpeed();
  static float GetZoom();
};