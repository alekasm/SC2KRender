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
  static HWND MouseSensBar, MoveSpeedBar, FOVBar, RenderDistBar;
  static HWND MouseSensText, MoveSpeedText, FOVText, RenderDistText;
  static HMENU Menu, FileMenu, OptionsMenu;
  static RECT WindowRect;
  static RECT ClientRect;
  static RECT SettingsRect;
  static HWND ShowDebugUICheckbox, AABBFrustumCullingCheckbox,
              VSyncCheckbox;
  static HWND MSAAComboBox;

  static void UpdateWindows()
  {
    UpdateWindow(MouseSensBar);
    UpdateWindow(MoveSpeedBar);
    UpdateWindow(FOVBar);
    UpdateWindow(MouseSensText);
    UpdateWindow(MoveSpeedText);
    UpdateWindow(FOVText);
    UpdateWindow(ShowDebugUICheckbox);
    UpdateWindow(RenderDistBar);
    UpdateWindow(RenderDistText);
    UpdateWindow(AABBFrustumCullingCheckbox);
    UpdateWindow(MSAAComboBox);
  }

  static BOOL IsTextHWND(HWND hWnd)
  {
    return 
      hWnd == MouseSensText ||
      hWnd == MoveSpeedText ||
      hWnd == FOVText ||
      hWnd == RenderDistText;
  }
};