#pragma once
#include <Windows.h>

#define MENU_LOAD_MAP 1
#define MENU_SHOW_CONSOLE 2
#define MENU_SHOW_MODELS 3
#define MENU_SHOW_SETTINGS 4
#define MENU_EXIT 5
#define MENU_SCREEN_WINDOWED 6
#define MENU_SCREEN_BORDERLESS 7
#define MENU_SCREEN_FULLSCREEN 8
#define MENU_SHOW_HELP 9


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
enum ScreenMode{WINDOWED, BORDERLESS, FULLSCREEN};
struct MenuContext
{
  static HWND hWndClient, hWndSettings;
  static HWND MouseSensBar, MoveSpeedBar, FOVBar, RenderDistBar;
  static HWND MouseSensText, MoveSpeedText, FOVText, RenderDistText;
  static HMENU Menu, FileMenu, OptionsMenu, ResolutionMenu;
  static RECT WindowRect;
  static RECT ClientRect;
  static RECT SettingsRect;
  static HWND ShowDebugUICheckbox, VSyncCheckbox; //, AABBFrustumCullingCheckbox;
  static HWND MSAAComboBox;
  static ScreenMode screen_mode;
  

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
    UpdateWindow(VSyncCheckbox);
    //UpdateWindow(AABBFrustumCullingCheckbox);
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