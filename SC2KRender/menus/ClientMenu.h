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

void Menus::InitializeClientMenu(HINSTANCE hInstance)
{
  DWORD grfStyle, grfExStyle;
  grfStyle = WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  grfExStyle = WS_EX_STATICEDGE;
  AdjustWindowRectEx(&MenuContext::ClientRect, grfStyle, TRUE, grfExStyle);

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
    grfExStyle,
    MainClass.lpszClassName, std::string("SC2KRender").c_str(),
    grfStyle,
    0, 0,
    MenuContext::ClientRect.right - MenuContext::ClientRect.left,
    MenuContext::ClientRect.bottom - MenuContext::ClientRect.top,
    NULL, NULL, hInstance, NULL);
  
  MenuContext::Menu = CreateMenu();
  MenuContext::FileMenu = CreateMenu();
  MenuContext::OptionsMenu = CreateMenu();

  AppendMenu(MenuContext::Menu, MF_POPUP, (UINT_PTR)MenuContext::FileMenu, "File");
  AppendMenu(MenuContext::FileMenu, MF_STRING, MENU_LOAD_MAP, "Load Map");
  AppendMenu(MenuContext::FileMenu, MF_UNCHECKED, MENU_SHOW_CONSOLE, "Show Console");
  AppendMenu(MenuContext::Menu, MF_POPUP, (UINT_PTR)MenuContext::OptionsMenu, "Options");
  AppendMenu(MenuContext::OptionsMenu, MF_CHECKED, MENU_SHOW_MODELS, "Render Models");
  AppendMenu(MenuContext::OptionsMenu, MF_UNCHECKED , MENU_SHOW_SETTINGS, "Settings..."); //| MF_GRAYED
  SetMenu(MenuContext::hWndClient, MenuContext::Menu);
}