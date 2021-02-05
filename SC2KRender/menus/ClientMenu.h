#pragma once
#include "MenuContext.h"
#include "Menus.h"
#include "../resource.h"
#include <string>

HWND MenuContext::hWndClient = NULL;
RECT MenuContext::ClientRect = { 0, 0, 1024, 768 };
RECT MenuContext::WindowRect = {};
HMENU MenuContext::Menu = NULL;
HMENU MenuContext::FileMenu = NULL;
HMENU MenuContext::OptionsMenu = NULL;
HMENU MenuContext::ResolutionMenu = NULL;
ScreenMode MenuContext::screen_mode = ScreenMode::WINDOWED;

void Menus::SetWindowedMode()
{
  if (MenuContext::screen_mode == ScreenMode::WINDOWED) return;
  SetWindowLongPtr(MenuContext::hWndClient, GWL_STYLE, WS_OVERLAPPEDWINDOW);
  SetWindowLongPtr(MenuContext::hWndClient, GWL_EXSTYLE, 0);
  ShowWindow(MenuContext::hWndClient, SW_SHOWNORMAL);
  SetWindowPos(MenuContext::hWndClient, HWND_TOP,
    0, 0,
    MenuContext::ClientRect.right - MenuContext::ClientRect.left,
    MenuContext::ClientRect.bottom - MenuContext::ClientRect.top,
    SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
  MenuContext::screen_mode = ScreenMode::WINDOWED;
}

void Menus::SetBorderlessMode()
{
  if (MenuContext::screen_mode == ScreenMode::BORDERLESS) return;
  SetWindowLongPtr(MenuContext::hWndClient, GWL_STYLE, 0);
  SetWindowLongPtr(MenuContext::hWndClient, GWL_EXSTYLE, WS_EX_TOPMOST);
  SetWindowPos(MenuContext::hWndClient, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
  ShowWindow(MenuContext::hWndClient, SW_SHOWMAXIMIZED);
  MenuContext::screen_mode = ScreenMode::BORDERLESS;
}

void Menus::SetFullScreenMode()
{
  MenuContext::screen_mode = ScreenMode::FULLSCREEN;
}

void Menus::InitializeClientMenu(HINSTANCE hInstance)
{
  AdjustWindowRectEx(&MenuContext::ClientRect, WS_OVERLAPPEDWINDOW, TRUE, 0);
  WNDCLASSEX MainClass = {};
  MainClass.cbClsExtra = NULL;
  MainClass.cbWndExtra = NULL;
  MainClass.lpszMenuName = NULL;
  MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  MainClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  MainClass.cbSize = sizeof(WNDCLASSEX);
  MainClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  MainClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  MainClass.hInstance = hInstance;
  MainClass.lpfnWndProc = WndProc;
  MainClass.lpszClassName = "ClientClass";
  MainClass.style = CS_HREDRAW | CS_VREDRAW;
  RegisterClassEx(&MainClass);

  MenuContext::hWndClient = CreateWindowEx(
    0,
    MainClass.lpszClassName, std::string("SC2KRender").c_str(),
    WS_OVERLAPPEDWINDOW,
    0, 0,
    MenuContext::ClientRect.right - MenuContext::ClientRect.left,
    MenuContext::ClientRect.bottom - MenuContext::ClientRect.top,
    NULL, NULL, hInstance, NULL);
  
  MenuContext::Menu = CreateMenu();
  MenuContext::FileMenu = CreateMenu();
  MenuContext::OptionsMenu = CreateMenu();
  MenuContext::ResolutionMenu = CreateMenu();

  AppendMenu(MenuContext::Menu, MF_POPUP, (UINT_PTR)MenuContext::FileMenu, "File");
  AppendMenu(MenuContext::FileMenu, MF_STRING, MENU_LOAD_MAP, "Load Map");
  AppendMenu(MenuContext::FileMenu, MF_UNCHECKED, MENU_SHOW_CONSOLE, "Show Console");
  AppendMenu(MenuContext::FileMenu, MF_STRING, MENU_SHOW_HELP, "Help");
  AppendMenu(MenuContext::FileMenu, MF_STRING, MENU_EXIT, "Exit");

  AppendMenu(MenuContext::Menu, MF_POPUP, (UINT_PTR)MenuContext::OptionsMenu, "Options");
  AppendMenu(MenuContext::OptionsMenu, MF_CHECKED, MENU_SHOW_MODELS, "Render Models");
  AppendMenu(MenuContext::OptionsMenu, MF_UNCHECKED , MENU_SHOW_SETTINGS, "Settings..."); //| MF_GRAYED

  AppendMenu(MenuContext::OptionsMenu, MF_POPUP, (UINT_PTR)MenuContext::ResolutionMenu, "Screen Mode");

  AppendMenu(MenuContext::ResolutionMenu, MF_CHECKED, MENU_SCREEN_WINDOWED, "Windowed");
  AppendMenu(MenuContext::ResolutionMenu, MF_UNCHECKED, MENU_SCREEN_BORDERLESS, "Borderless");
  AppendMenu(MenuContext::ResolutionMenu, MF_UNCHECKED, MENU_SCREEN_FULLSCREEN, "Fullscreen");

  SetMenu(MenuContext::hWndClient, MenuContext::Menu);
}