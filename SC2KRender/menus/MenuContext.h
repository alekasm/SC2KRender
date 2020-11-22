#pragma once
#include <Windows.h>

#define MENU_LOAD_MAP 1
#define MENU_SHOW_CONSOLE 2
#define MENU_SHOW_MODELS 3
#define MENU_SHOW_SETTINGS 4

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
struct MenuContext
{
  static HWND hWndClient, hWndSettings;
  static HWND MouseSensBar, MoveSpeedBar, ZoomBar, RenderDistBar;
  static HWND MouseSensText, MoveSpeedText, ZoomText, RenderDistText;
  static HMENU Menu, FileMenu, OptionsMenu;
  static RECT WindowRect;
  static RECT ClientRect;
  static RECT SettingsRect;
  static HWND ShowDebugUICheckbox, AABBFrustumCullingCheckbox;

  static void UpdateWindows()
  {
    UpdateWindow(MouseSensBar);
    UpdateWindow(MoveSpeedBar);
    UpdateWindow(ZoomBar);
    UpdateWindow(MouseSensText);
    UpdateWindow(MoveSpeedText);
    UpdateWindow(ZoomText);
    UpdateWindow(ShowDebugUICheckbox);
    UpdateWindow(RenderDistBar);
    UpdateWindow(RenderDistText);
    UpdateWindow(AABBFrustumCullingCheckbox);
  }

  static BOOL IsTextHWND(HWND hWnd)
  {
    return 
      hWnd == MouseSensText ||
      hWnd == MoveSpeedText ||
      hWnd == ZoomText ||
      hWnd == RenderDistText;
  }
};