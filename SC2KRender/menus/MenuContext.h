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
  static HWND MouseSensBar, MoveSpeedBar, ZoomBar;
  static HWND MouseSensText, MoveSpeedText, ZoomText;
  static HMENU Menu, FileMenu, OptionsMenu;
  static RECT WindowRect;
  static RECT ClientRect;
  static RECT SettingsRect;
  static HWND ShowDebugUICheckbox;

  static void UpdateWindows()
  {
    UpdateWindow(MouseSensBar);
    UpdateWindow(MoveSpeedBar);
    UpdateWindow(ZoomBar);
    UpdateWindow(MouseSensText);
    UpdateWindow(MoveSpeedText);
    UpdateWindow(ZoomText);
    UpdateWindow(ShowDebugUICheckbox);
  }

  static BOOL IsTextHWND(HWND hWnd)
  {
    return 
      hWnd == MouseSensText ||
      hWnd == MoveSpeedText ||
      hWnd == ZoomText;
  }
};