/*
Created by Aleksander Krimsky

Thanks/Extra Credits:
fr0zi - https://github.com/fr0zi/sccopter
bzroom (GameDev.net Discord) - DirectX Help
slicer4ever (GameDev.net Discord) - DirectX Help
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
#include "resource.h"

#define MENU_LOAD_MAP 1
#define MENU_SHOW_CONSOLE 2
#define MENU_SHOW_MODELS 3
#define MENU_SHOW_TREES 4

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hWndClient;
RECT WindowRect;
RECT ClientRect = {0, 0, 1024, 768};
MapTile* tiles = nullptr;
std::unique_ptr<Scene> scene;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	AllocConsole();
	FILE* p_file;
	freopen_s(&p_file, "CONIN$", "r", stdin);
	freopen_s(&p_file, "CONOUT$", "w", stdout);
	freopen_s(&p_file, "CONOUT$", "w", stderr);
	std::cout << "SC2KRender (Version 0.1) by Aleksander Krimsky | www.krimsky.net" << std::endl;

	int argc = 0;
	LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argc);	
	std::wstring wfilename = L"";
	if (argc == 2)
	{
		wfilename = std::wstring(args[1]);
	}
	LocalFree(args);
	scene = std::make_unique<Scene>();
	
	//std::string filename = "";
	/*
	do
	{		
		if (wfilename.empty())
		{
			std::cout << "Enter a SimCity 2000 Map to load: ";
			std::wcin >> wfilename;
		}		
		int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wfilename[0], wfilename.size(), NULL, 0, NULL, NULL);
		filename = std::string(size, 0);
		WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wfilename[0], wfilename.size(), &filename[0], size, NULL, NULL);
		wfilename = L"";
	} while (!MapLoader::LoadMap(filename, tiles));
	*/


	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	DWORD grfStyle, grfExStyle;
	grfStyle = WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	grfExStyle = WS_EX_STATICEDGE;
	AdjustWindowRectEx(&ClientRect, grfStyle, TRUE, grfExStyle);

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
	MainClass.lpszClassName = "WindowClassName";
	MainClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&MainClass);

	hWndClient = CreateWindowEx(
		grfExStyle,
		MainClass.lpszClassName, std::string("SC2KRender").c_str(),
		grfStyle,
		0, 0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, NULL, NULL, hInstance, NULL);



	HMENU Menu = CreateMenu();
	HMENU FileMenu = CreateMenu();
	HMENU AboutMenu = CreateMenu();

	AppendMenu(Menu, MF_POPUP, (UINT_PTR)FileMenu, "File");
	AppendMenu(Menu, MF_POPUP, (UINT_PTR)AboutMenu, "About");
	AppendMenu(FileMenu, MF_STRING, MENU_LOAD_MAP, "Load Map");
	AppendMenu(FileMenu, MF_CHECKED, MENU_SHOW_MODELS, "Show Debug");
	AppendMenu(FileMenu, MF_CHECKED, MENU_SHOW_MODELS, "Render Models (Beta)");
	AppendMenu(FileMenu, MF_UNCHECKED, MENU_SHOW_TREES, "Render Trees (Beta)");
	SetMenu(hWndClient, Menu);

	scene->PreInitialize(hWndClient);
	ShowWindow(hWndClient, TRUE);	

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (GetActiveWindow() == hWndClient)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	scene.reset();
	CoUninitialize();
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_WINDOWPOSCHANGED:		
		if (scene->Initialized())
		{			
			scene->UpdateWindow(hWnd);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case MENU_LOAD_MAP:
		{
			::SetFocus(NULL);
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
			printf("Loading map: %s\n", filename.c_str());
			if (!filename.empty() && MapLoader::LoadMap(filename, tiles))
			{
				scene->Initialize(tiles);
				scene->SetFocus(true);
				SetWindowTextA(hWndClient, std::string("SC2KRender - " + filename).c_str());
			}
		}
		break;
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
			scene->MouseLook(p.x, 0, p.y);
		}
		break;
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
				GetWindowRect(hWndClient, &rc);
				if (PtInRect(&rc, p))
				{
					scene->SetFocus(true);					
				}
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}