#pragma once
#include "MenuContext.h"
#include "Menus.h"
#include "../resource.h"
#include <CommCtrl.h> //CommCtrl includes sliders
#include <Windowsx.h> //Button_SetCheck
#include <d3d11_1.h>
#include <string>

RECT MenuContext::SettingsRect = { 0, 0, 420, 270 };
HWND MenuContext::MouseSensText = NULL;
HWND MenuContext::MoveSpeedText = NULL;
HWND MenuContext::FOVText = NULL;
HWND MenuContext::MouseSensBar = NULL;
HWND MenuContext::MoveSpeedBar = NULL;
HWND MenuContext::RenderDistBar = NULL;
HWND MenuContext::RenderDistText = NULL;

HWND MenuContext::FOVBar = NULL;
HWND MenuContext::ShowDebugUICheckbox = NULL;
//HWND MenuContext::AABBFrustumCullingCheckbox = NULL;
HWND MenuContext::MSAAComboBox = NULL;
HWND MenuContext::VSyncCheckbox = NULL;
HWND MenuContext::hWndSettings = NULL;

const std::string speed_text_values[7] = { "Slowest", "Slower", "Slow", "Normal", "Fast", "Faster", "Fastest" };
const float move_speed_values[7] = {0.01f, 0.05f, 0.1f, 0.2f, 0.3f, 0.35f, 0.75f};
const float mouse_speed_values[7] = {0.00025f, 0.001f, 0.002f, 0.003f, 0.004f, 0.005f, 0.01f};
const float render_dist_values[7] = { 16.f, 32.f, 48.f, 64.f, 96.f, 128.f, 0.f };

float mouse_speed_value;
float move_speed_value;
float fov_value;
float render_dist_value; //each tile is .5f, so 2 tiles = 1.f, each 0.5m = 50m

void Menus::SetMaxSamples(unsigned int count)
{
  unsigned int index = 0;
  for (unsigned int samples = 2; samples <= count; samples *= 2)
  {
    char text[64];
    snprintf(text, sizeof(text), "MSAA %dx", samples);
    SendMessage(MenuContext::MSAAComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)text);   
    ++index;
  }  
  SendMessage(MenuContext::MSAAComboBox, CB_SETCURSEL, (WPARAM)index, (LPARAM)0);
  UpdateWindow(MenuContext::MSAAComboBox);
}

void Menus::UpdateMouseSpeedBar(int slider_value)
{
  if (slider_value < 1 || slider_value > 7) return;
  mouse_speed_value = mouse_speed_values[slider_value - 1];

  std::string mouse_speed_text("Mouse Sensitivity: ");
  mouse_speed_text.append(speed_text_values[slider_value - 1]);
  SetWindowText(MenuContext::MouseSensText, mouse_speed_text.c_str());
  UpdateWindow(MenuContext::MouseSensText);
}

void Menus::UpdateMoveSpeedBar(int slider_value)
{
  if (slider_value < 1 || slider_value > 7) return;
  move_speed_value = move_speed_values[slider_value - 1];

  std::string move_speed_text("Movement Speed: ");
  move_speed_text.append(speed_text_values[slider_value - 1]);
  SetWindowText(MenuContext::MoveSpeedText, move_speed_text.c_str());
  UpdateWindow(MenuContext::MoveSpeedText);
}

void Menus::UpdateFOVBar(int slider_value)
{
  if (slider_value < 1 || slider_value > 7) return;
  fov_value = 50.f + ((float) slider_value * 10.f);

  std::string fov_text("Field of View: ");
  fov_text.append(std::to_string((int)fov_value));
  SetWindowText(MenuContext::FOVText, fov_text.c_str());
  UpdateWindow(MenuContext::FOVText);
}

void Menus::UpdateRenderDistBar(int slider_value)
{
  if (slider_value < 1 || slider_value > 7) return;
  render_dist_value = render_dist_values[slider_value - 1];

  std::string render_dist_text("Render Distance: ");
  render_dist_text.append(render_dist_value ? 
    std::to_string((int)render_dist_value * 50).append("m") : "Maximum");
  SetWindowText(MenuContext::RenderDistText, render_dist_text.c_str());
  UpdateWindow(MenuContext::RenderDistText);
}

float Menus::GetMoveSpeed()
{
  return move_speed_value;
}

float Menus::GetMouseSpeed()
{
  return mouse_speed_value;
}

float Menus::GetFOV()
{
  return fov_value;
}

float Menus::GetRenderDist()
{
  return render_dist_value;
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
  SendMessage(MenuContext::MouseSensBar, TBM_SETRANGEMAX, WPARAM(FALSE), LPARAM(7));
  SendMessage(MenuContext::MouseSensBar, TBM_SETPOS, WPARAM(FALSE), LPARAM(4));
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
  SendMessage(MenuContext::MoveSpeedBar, TBM_SETRANGEMAX, WPARAM(FALSE), LPARAM(7));
  SendMessage(MenuContext::MoveSpeedBar, TBM_SETPOS, WPARAM(FALSE), LPARAM(4));
  SendMessage(MenuContext::MoveSpeedBar, TBM_SETTICFREQ, WPARAM(1), LPARAM(0));
  UpdateWindow(MenuContext::MoveSpeedBar);

  MenuContext::MoveSpeedText = CreateWindow("EDIT", "Movement Speed: Normal",
    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE,
    190, 50, 290, 20, MenuContext::hWndSettings, NULL, NULL, NULL);
  UpdateWindow(MenuContext::MoveSpeedText);

  MenuContext::FOVBar = CreateWindow(
    TRACKBAR_CLASS, "FOVBar", WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS,
    10, 90, 150, 20, MenuContext::hWndSettings, NULL, NULL, NULL);

  SendMessage(MenuContext::FOVBar, TBM_SETRANGEMIN, WPARAM(FALSE), LPARAM(1));
  SendMessage(MenuContext::FOVBar, TBM_SETRANGEMAX, WPARAM(FALSE), LPARAM(7));
  SendMessage(MenuContext::FOVBar, TBM_SETPOS, WPARAM(FALSE), LPARAM(2));
  SendMessage(MenuContext::FOVBar, TBM_SETTICFREQ, WPARAM(1), LPARAM(0));
  UpdateWindow(MenuContext::FOVBar);

  MenuContext::FOVText = CreateWindow("EDIT", "Field of View: 70",
    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE,
    190, 90, 290, 20, MenuContext::hWndSettings, NULL, NULL, NULL);
  UpdateWindow(MenuContext::FOVText);

  MenuContext::RenderDistBar = CreateWindow(
    TRACKBAR_CLASS, "RenderDistBar", WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS,
    10, 130, 150, 20, MenuContext::hWndSettings, NULL, NULL, NULL);

  SendMessage(MenuContext::RenderDistBar, TBM_SETRANGEMIN, WPARAM(FALSE), LPARAM(1));
  SendMessage(MenuContext::RenderDistBar, TBM_SETRANGEMAX, WPARAM(FALSE), LPARAM(7));
  SendMessage(MenuContext::RenderDistBar, TBM_SETPOS, WPARAM(FALSE), LPARAM(7));
  SendMessage(MenuContext::RenderDistBar, TBM_SETTICFREQ, WPARAM(1), LPARAM(0));
  UpdateWindow(MenuContext::RenderDistBar);

  MenuContext::RenderDistText = CreateWindow("EDIT", "Render Distance:",
    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE,
    190, 130, 290, 20, MenuContext::hWndSettings, NULL, NULL, NULL);
  UpdateWindow(MenuContext::RenderDistText);

  MenuContext::MSAAComboBox = CreateWindow(
    "COMBOBOX", "", WS_VISIBLE | WS_CHILDWINDOW | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_BORDER,
    10, 170, 150, 150, MenuContext::hWndSettings, NULL, NULL, NULL);

  SendMessage(MenuContext::MSAAComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"No Anti-Aliasing");
  SendMessage(MenuContext::MSAAComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

  /*
  MenuContext::AABBFrustumCullingCheckbox = CreateWindow(
    "Button", "AABB Frustum Culling", WS_VISIBLE | WS_CHILDWINDOW | BS_AUTOCHECKBOX,
    180, 170, 200, 25, MenuContext::hWndSettings, NULL,
    NULL, NULL);
  Button_SetCheck(MenuContext::AABBFrustumCullingCheckbox, BST_CHECKED);
  */

  MenuContext::ShowDebugUICheckbox = CreateWindow(
    "Button", "Enable Debug", WS_VISIBLE | WS_CHILDWINDOW | BS_AUTOCHECKBOX,
    10, 195, 150, 25, MenuContext::hWndSettings, NULL,
    NULL, NULL);
  Button_SetCheck(MenuContext::ShowDebugUICheckbox, BST_UNCHECKED);

  MenuContext::VSyncCheckbox = CreateWindow(
    "Button", "Enable VSync", WS_VISIBLE | WS_CHILDWINDOW | BS_AUTOCHECKBOX,
    180, 195, 150, 25, MenuContext::hWndSettings, NULL,
    NULL, NULL);
  Button_SetCheck(MenuContext::VSyncCheckbox, BST_CHECKED);


  UpdateMoveSpeedBar(4);
  UpdateMouseSpeedBar(4);
  UpdateRenderDistBar(7);
  UpdateFOVBar(2);
}