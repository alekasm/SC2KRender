#pragma once
#include "MenuContext.h"
#include "Menus.h"
#include "../resource.h"
#include <CommCtrl.h> //CommCtrl includes sliders
#include <Windowsx.h> //Button_SetCheck
#include <string>

RECT MenuContext::SettingsRect = { 0, 0, 420, 220 };
HWND MenuContext::MouseSensText = NULL;
HWND MenuContext::MoveSpeedText = NULL;
HWND MenuContext::ZoomText = NULL;
HWND MenuContext::MouseSensBar = NULL;
HWND MenuContext::MoveSpeedBar = NULL;
HWND MenuContext::ZoomBar = NULL;
HWND MenuContext::ShowDebugUICheckbox = NULL;
HWND MenuContext::hWndSettings = NULL;
const std::string speed_text_values[5] = { "Very Slow", "Slow", "Normal", "Fast", "Very Fast" };
const float move_speed_values[5] = {0.05f, 0.1f, 0.2f, 0.3f, 0.35f};
const float mouse_speed_values[5] = {0.001f, 0.002f, 0.003f, 0.004f, 0.005f};

float mouse_speed_value;
float move_speed_value;
float zoom_value;

void Menus::UpdateMouseSpeedBar(int slider_value)
{
  if (slider_value < 1 || slider_value > 5) return;
  mouse_speed_value = mouse_speed_values[slider_value - 1];

  std::string mouse_speed_text("Mouse Sensitivity: ");
  mouse_speed_text.append(speed_text_values[slider_value - 1]);
  SetWindowText(MenuContext::MouseSensText, mouse_speed_text.c_str());
  UpdateWindow(MenuContext::MouseSensText);
}

void Menus::UpdateMoveSpeedBar(int slider_value)
{
  if (slider_value < 1 || slider_value > 5) return;
  move_speed_value = move_speed_values[slider_value - 1];

  std::string move_speed_text("Movement Speed: ");
  move_speed_text.append(speed_text_values[slider_value - 1]);
  SetWindowText(MenuContext::MoveSpeedText, move_speed_text.c_str());
  UpdateWindow(MenuContext::MoveSpeedText);
}

void Menus::UpdateZoomBar(int slider_value)
{
  if (slider_value < 1 || slider_value > 10) return;
  zoom_value = (float) slider_value / 10.f;

  std::string zoom_text("World Zoom: ");
  zoom_text.append(std::to_string(slider_value * 10));
  zoom_text.append("%");
  SetWindowText(MenuContext::ZoomText, zoom_text.c_str());
  UpdateWindow(MenuContext::ZoomText);
}

float Menus::GetMoveSpeed()
{
  return move_speed_value;
}

float Menus::GetMouseSpeed()
{
  return mouse_speed_value;
}

float Menus::GetZoom()
{
  return zoom_value;
}

void Menus::InitializeSettingsMenu(HINSTANCE hInstance)
{  
  WNDCLASSEX SettingsClass;
  SettingsClass.cbClsExtra = NULL;
  SettingsClass.cbWndExtra = NULL;
  SettingsClass.lpszMenuName = NULL;
  SettingsClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  SettingsClass.hbrBackground = CreateSolidBrush(COLORREF(0xf0f0f0));
  SettingsClass.cbSize = sizeof(WNDCLASSEX);
  SettingsClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  SettingsClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  SettingsClass.hInstance = hInstance;
  SettingsClass.lpfnWndProc = WndProc;
  SettingsClass.lpszClassName = "SettingsClass";
  SettingsClass.style = CS_HREDRAW | CS_VREDRAW;
  RegisterClassEx(&SettingsClass);  
 
  MenuContext::hWndSettings = CreateWindowEx(
    WS_EX_STATICEDGE,
    SettingsClass.lpszClassName,
    std::string("SC2KRender Settings").c_str(),
    WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    0, 0, 
    MenuContext::SettingsRect.right - MenuContext::SettingsRect.left,
    MenuContext::SettingsRect.bottom - MenuContext::SettingsRect.top, NULL, NULL, NULL, NULL);

  ShowWindow(MenuContext::hWndSettings, FALSE);    

  MenuContext::MouseSensBar = CreateWindow(
    TRACKBAR_CLASS, "MouseSensBar", WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS,
    10, 10, 150, 20, MenuContext::hWndSettings, NULL, NULL, NULL);

  SendMessage(MenuContext::MouseSensBar, TBM_SETRANGEMIN, WPARAM(FALSE), LPARAM(1));
  SendMessage(MenuContext::MouseSensBar, TBM_SETRANGEMAX, WPARAM(FALSE), LPARAM(5));
  SendMessage(MenuContext::MouseSensBar, TBM_SETPOS, WPARAM(FALSE), LPARAM(3));
  SendMessage(MenuContext::MouseSensBar, TBM_SETTICFREQ, WPARAM(1), LPARAM(0));
  UpdateWindow(MenuContext::MouseSensBar);

  MenuContext::MouseSensText = CreateWindow("EDIT", "Mouse Sensitivity: Normal",
    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE,
    190, 10, 290, 20, MenuContext::hWndSettings, NULL, NULL, NULL);
  UpdateWindow(MenuContext::MouseSensText);

  MenuContext::MoveSpeedBar = CreateWindow(
    TRACKBAR_CLASS, "MoveSpeedBar", WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS,
    10, 50, 150, 20, MenuContext::hWndSettings, NULL, NULL, NULL);

  SendMessage(MenuContext::MoveSpeedBar, TBM_SETRANGEMIN, WPARAM(FALSE), LPARAM(1));
  SendMessage(MenuContext::MoveSpeedBar, TBM_SETRANGEMAX, WPARAM(FALSE), LPARAM(5));
  SendMessage(MenuContext::MoveSpeedBar, TBM_SETPOS, WPARAM(FALSE), LPARAM(3));
  SendMessage(MenuContext::MoveSpeedBar, TBM_SETTICFREQ, WPARAM(1), LPARAM(0));
  UpdateWindow(MenuContext::MoveSpeedBar);

  MenuContext::MoveSpeedText = CreateWindow("EDIT", "Movement Speed: Normal",
    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE,
    190, 50, 290, 20, MenuContext::hWndSettings, NULL, NULL, NULL);
  UpdateWindow(MenuContext::MoveSpeedText);

  MenuContext::ZoomBar = CreateWindow(
    TRACKBAR_CLASS, "ZoomBar", WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS,
    10, 90, 150, 20, MenuContext::hWndSettings, NULL, NULL, NULL);

  SendMessage(MenuContext::ZoomBar, TBM_SETRANGEMIN, WPARAM(FALSE), LPARAM(1));
  SendMessage(MenuContext::ZoomBar, TBM_SETRANGEMAX, WPARAM(FALSE), LPARAM(10));
  SendMessage(MenuContext::ZoomBar, TBM_SETPOS, WPARAM(FALSE), LPARAM(1));
  SendMessage(MenuContext::ZoomBar, TBM_SETTICFREQ, WPARAM(1), LPARAM(0));
  UpdateWindow(MenuContext::ZoomBar);

  MenuContext::ZoomText = CreateWindow("EDIT", "World Zoom: 10%",
    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE,
    190, 90, 290, 20, MenuContext::hWndSettings, NULL, NULL, NULL);
  UpdateWindow(MenuContext::ZoomText);

  MenuContext::ShowDebugUICheckbox = CreateWindow(
    "Button", "Show Debug UI", WS_VISIBLE | WS_CHILDWINDOW | BS_AUTOCHECKBOX,
    10, 130, 200, 25, MenuContext::hWndSettings, NULL,
    NULL, NULL);
  Button_SetCheck(MenuContext::ShowDebugUICheckbox, BST_CHECKED);

  UpdateMoveSpeedBar(3);
  UpdateMouseSpeedBar(3);
  UpdateZoomBar(1);
}