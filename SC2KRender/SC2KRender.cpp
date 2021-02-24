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
std::string help_string;
HCURSOR cursor;

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
  cursor = LoadCursor(NULL, IDC_ARROW);

  std::filesystem::path map_path = std::filesystem::current_path();
  map_path.append("maps");

  help_string = "SC2KRender Version 1.0\n";
  help_string.append("Written by Aleksander Krimsky | www.krimsky.net\n");
  help_string.append("Lead Artist: Thomas Nelson\n");
  help_string.append("Written with DirectX 11 - Modified DirectXTK\n");
  help_string.append("GitHub: https://github.com/alekasm/SC2KRender \n\n");
  help_string.append("Controls:\nFree Cam: WASD\nStrafe Up/Down: RF\n\n");
  if (std::filesystem::exists(map_path))
  {
    help_string.append("Maps: ");
    help_string.append(map_path.string());
    help_string.append("\n");
  }
  printf("%s", help_string.c_str());

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

//This function will be called when activating it, or when exiting fullscreen
void SetWindowedBorderless()
{
  MENUITEMINFO mii;
  mii.cbSize = sizeof(MENUITEMINFO);
  mii.fMask = MIIM_STATE;
  mii.fState = MFS_CHECKED;
  SetMenuItemInfo(MenuContext::ResolutionMenu, MENU_SCREEN_BORDERLESS, FALSE, &mii);
  mii.fState = MFS_UNCHECKED;
  SetMenuItemInfo(MenuContext::ResolutionMenu, MENU_SCREEN_WINDOWED, FALSE, &mii);
  SetMenuItemInfo(MenuContext::ResolutionMenu, MENU_SCREEN_FULLSCREEN, FALSE, &mii);
  Menus::SetBorderlessMode();
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {

  case WM_SETCURSOR: //Patches an unsolved edge case with ShowCursor(FALSE) 
      SetCursor(scene->HasFocus() ? NULL : cursor);
  break;

  case WM_KEYDOWN:
    switch (wParam)
    {
    case VK_ESCAPE:
    {
      if (MenuContext::screen_mode == ScreenMode::FULLSCREEN)
      {
        if (scene->SetFullScreen(FALSE))
        {
          SetWindowedBorderless();
        }
      }
      if (scene->HasFocus())
      {
        scene->SetFocus(false);
      }
      else
      {
        std::thread([] {
          if (MessageBox(MenuContext::hWndClient,
            "Do you really want to quit?", "SC2KRender Quit",
            MB_YESNO | MB_ICONQUESTION) == IDYES)
          {
            SendMessage(MenuContext::hWndClient, WM_DESTROY, 0, 0);
          }
        }).detach();
      }
    }
    break;
    case VK_F11:
    {
      switch (MenuContext::screen_mode)
      {
      case FULLSCREEN:
        if (scene->SetFullScreen(FALSE))
        {
          SetWindowedBorderless();
        }
        break;
      default:
        SetWindowedBorderless();
        if (scene->SetFullScreen(TRUE))
        {
          Menus::SetFullScreenMode();
        }
      }
      scene->SetFocus(true);
    }
    break;

    }
    break;    

  case WM_WINDOWPOSCHANGED:
    if (hWnd == MenuContext::hWndClient)
    {
      if (MenuContext::screen_mode != ScreenMode::FULLSCREEN)
      {
        scene->UpdateWindow(hWnd);
      }
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
      else if ((HWND)lParam == MenuContext::MSAAComboBox)
      {
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
          unsigned int index = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
          scene->SetMSAA(index);          
        }
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

    case MENU_EXIT:
      PostQuitMessage(0);
      break;

    case MENU_SHOW_HELP:       
      std::thread([] { //A really terrible workaround
        MessageBox(MenuContext::hWndClient, help_string.c_str(), 
          "SC2KRender Help", MB_OK | MB_ICONASTERISK);
        //return DefWindowProc(hWnd, message, wParam, lParam);
      }).detach();    
    break;
    
    case MENU_SHOW_SETTINGS:
      ShowWindow(MenuContext::hWndSettings, TRUE);
      break;

    case MENU_SCREEN_WINDOWED:
    {
      MENUITEMINFO mii;
      mii.cbSize = sizeof(MENUITEMINFO);
      mii.fMask = MIIM_STATE;
      mii.fState = MFS_CHECKED;
      SetMenuItemInfo(MenuContext::ResolutionMenu, MENU_SCREEN_WINDOWED, FALSE, &mii);
      mii.fState = MFS_UNCHECKED;
      SetMenuItemInfo(MenuContext::ResolutionMenu, MENU_SCREEN_BORDERLESS, FALSE, &mii);
      SetMenuItemInfo(MenuContext::ResolutionMenu, MENU_SCREEN_FULLSCREEN, FALSE, &mii);
      Menus::SetWindowedMode();
    }
    break;

    case MENU_SCREEN_BORDERLESS:
      SetWindowedBorderless();
      break;

    case MENU_SCREEN_FULLSCREEN:
      SetWindowedBorderless();
      if (scene->SetFullScreen(TRUE))
      {
        Menus::SetFullScreenMode();  
        scene->SetFocus(true);
      }
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