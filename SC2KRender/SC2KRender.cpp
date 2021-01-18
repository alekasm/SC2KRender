/*
Created by Aleksander Krimsky

Thanks/Extra Credits:
fr0zi - https://github.com/fr0zi/sccopter
bzroom (GameDev.net Discord) - DirectX Help
slicer4ever (GameDev.net Discord) - DirectX Help
Thomas Nelson - Models
Frustum Culling: Chris Serson 
http://thedemonthrone.ca/projects/rendering-terrain/rendering-terrain-part-10-view-frustum-culling/
*/

#include <vector>
#include <ctime>
#include <thread>
#include <memory>
#include <fcntl.h>
#include <io.h>
#include <cstdio>
#include <iostream>
#include "MapLoader.h"
#include "Scene.h"
#include <conio.h>
#include <filesystem>
#include <CommCtrl.h> 

#include "menus/MenuContext.h"
#include "menus/Menus.h"


std::unique_ptr<Scene> scene;

extern "C"
{
  __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);


  AllocConsole();
  FILE* p_file;
  freopen_s(&p_file, "CONIN$", "r", stdin);
  freopen_s(&p_file, "CONOUT$", "w", stdout);
  freopen_s(&p_file, "CONOUT$", "w", stderr);

  //Prevent Quick-Edit mode, issue with message passthrough
  HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
  DWORD dwMode;
  GetConsoleMode(hInput, &dwMode);
  SetConsoleMode(hInput, dwMode & ENABLE_EXTENDED_FLAGS);
  ShowWindow(GetConsoleWindow(), SW_HIDE);

  printf("SC2KRender (Version 0.7)\n");
  printf("Written by Aleksander Krimsky | www.krimsky.net\n");
  printf("3D Models by Thomas Nelson\n");
  printf("Written with DirectX 11 - Modified DirectXTK\n");
  printf("GitHub: https://github.com/alekasm/SC2KRender \n");
  printf("\nControls:\nFree Cam: WASD\nStrafe Up/Down: RF\n\n");
  CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
  scene = std::make_unique<Scene>();

  Menus::InitializeClientMenu(hInstance);
  Menus::InitializeSettingsMenu(hInstance);

  scene->PreInitialize(MenuContext::hWndClient);
  Menus::SetMaxSamples(scene->GetMaxSampleCount());

  ShowWindow(MenuContext::hWndClient, TRUE);

  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  scene.reset();
  CoUninitialize();
  return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE)
    {
      if (scene->HasFocus())
      {
        scene->SetFocus(false);
      }
      else
      {
        PostQuitMessage(0);
      }
    }
    break;
    

  case WM_WINDOWPOSCHANGED:
    if (hWnd == MenuContext::hWndClient)
    {
      scene->UpdateWindow(hWnd);
    }    
    break;
  case WM_CLOSE:
    if (hWnd == MenuContext::hWndSettings)
    {
      ShowWindow(hWnd, SW_HIDE);
      return 0;
    }
    else if (hWnd == MenuContext::hWndClient)
    {
      DestroyWindow(hWnd);
    }
    break;
  case WM_COMMAND:
    if (hWnd == MenuContext::hWndSettings)
    {
      if (MenuContext::IsTextHWND((HWND)lParam))
      {
        ::SetFocus(NULL);
      }
      else if ((HWND)lParam == MenuContext::ShowDebugUICheckbox)
      {
        LRESULT chkState = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
        scene->SetRenderDebugUI(chkState == BST_CHECKED);
      }
      else if ((HWND)lParam == MenuContext::AABBFrustumCullingCheckbox)
      {
        LRESULT chkState = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
        scene->SetAABBFrustumCulling(chkState == BST_CHECKED);
      }
      else if ((HWND)lParam == MenuContext::MSAA4XCheckbox)
      {
        LRESULT chkState = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
        scene->SetMSAA(chkState == BST_CHECKED);
      }
      else if ((HWND)lParam == MenuContext::VSyncCheckbox)
      {
        LRESULT chkState = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
        scene->SetEnableVSync(chkState == BST_CHECKED);
      }
      break;
    }
    //Assume hWndClient
    switch (LOWORD(wParam))
    {
    case MENU_LOAD_MAP:
    {
      char szFile[256];
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(ofn));
      ofn.lStructSize = sizeof(ofn);
      ofn.hwndOwner = NULL;
      ofn.lpstrFile = szFile;
      ofn.lpstrFile[0] = '\0';
      ofn.nMaxFile = sizeof(szFile);
      ofn.lpstrFilter = "SimCity 2000 Map File\0*.sc2;*.SC2\0";
      ofn.nFilterIndex = 1;
      ofn.lpstrFileTitle = NULL;
      ofn.nMaxFileTitle = 0;
      ofn.lpstrInitialDir = NULL;
      ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
      GetOpenFileName(&ofn);
      std::string filename(ofn.lpstrFile);
      Map map;
      if (!filename.empty() && MapLoader::LoadMap(filename, map))
      {
        SetWindowTextA(MenuContext::hWndClient, std::string("SC2KRender - " + filename).c_str());
        scene->Initialize(map);
        scene->SetFocus(true);
      }
    }
    break;
    case MENU_SHOW_CONSOLE:
      MENUITEMINFO mii;
      mii.cbSize = sizeof(MENUITEMINFO);
      mii.fMask = MIIM_STATE;
      mii.fState = MFS_CHECKED | MFS_DISABLED;
      SetMenuItemInfo(MenuContext::FileMenu, MENU_SHOW_CONSOLE, FALSE, &mii);
      ShowWindow(GetConsoleWindow(), SW_SHOWNOACTIVATE);
      break;

    case MENU_SHOW_SETTINGS:
      ShowWindow(MenuContext::hWndSettings, TRUE);
      break;

    case MENU_SHOW_MODELS:
    {
      MENUITEMINFO mii;
      mii.cbSize = sizeof(MENUITEMINFO);
      mii.fMask = MIIM_STATE;
      GetMenuItemInfo(MenuContext::OptionsMenu, MENU_SHOW_MODELS, FALSE, &mii);
      if (mii.fState & MFS_CHECKED)
      {
        scene->SetRenderModels(false);
        mii.fState = MFS_UNCHECKED;
      }
      else
      {
        scene->SetRenderModels(true);
        mii.fState = MFS_CHECKED;
      }
      SetMenuItemInfo(MenuContext::OptionsMenu, MENU_SHOW_MODELS, FALSE, &mii);
    }
    break;

    }
    break;

  case WM_ACTIVATE:
    if (hWnd == MenuContext::hWndSettings)
      MenuContext::UpdateWindows();
    break;

  case WM_HSCROLL:
  {
    int slider_value = SendMessage((HWND)lParam, (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0);
    if ((HWND)lParam == MenuContext::MouseSensBar)
    {
      Menus::UpdateMouseSpeedBar(slider_value);
      scene->SetMouseSpeed(Menus::GetMouseSpeed());
    }
    else if ((HWND)lParam == MenuContext::MoveSpeedBar)
    {
      Menus::UpdateMoveSpeedBar(slider_value);
      scene->SetMovementSpeed(Menus::GetMoveSpeed());
    }
    else if ((HWND)lParam == MenuContext::FOVBar)
    {
      Menus::UpdateFOVBar(slider_value);
      scene->SetFOV(Menus::GetFOV());
    }
    else if ((HWND)lParam == MenuContext::RenderDistBar)
    {
      Menus::UpdateRenderDistBar(slider_value);
      scene->SetRenderDistance(Menus::GetRenderDist());
    }
  }
  break;

  case WM_ERASEBKGND:
    break;

  case WM_PAINT:
    scene->Tick();
    break;

  case WM_MOUSEMOVE:
    if (scene->HasFocus())
    {
      POINT p;
      GetCursorPos(&p);
      scene->MouseLook(p.x, p.y);
    }
    break;

  case WM_LBUTTONDOWN:
    if (wParam == MK_LBUTTON)
    {
      if (scene->HasFocus())
      {
        scene->MouseClick();
      }
      else
      {
        POINT p;
        GetCursorPos(&p);
        RECT rc;
        GetWindowRect(MenuContext::hWndClient, &rc);
        if (PtInRect(&rc, p))
        {
          scene->SetFocus(true);
        }
      }
    }
    break;
  case WM_DESTROY:
    if (hWnd == MenuContext::hWndClient)
      PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return hWnd == MenuContext::hWndClient ? 0 : DefWindowProc(hWnd, message, wParam, lParam);
}