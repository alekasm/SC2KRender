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



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;
RECT WindowRect;
RECT ClientRect = {0, 0, 1024, 768};

namespace
{
	std::unique_ptr<Scene> scene;
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
	std::cout << "SC2KRender (Version 0.1) by Aleksander Krimsky | www.krimsky.net" << std::endl;

	int argc = 0;
	LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argc);	
	std::wstring wfilename = L"";
	if (argc == 2)
	{
		wfilename = std::wstring(args[1]);
	}
	LocalFree(args);
	MapTile* tiles = nullptr;
	std::string filename = "";
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


	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	DWORD grfStyle, grfExStyle;
	grfStyle = WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	grfExStyle = WS_EX_STATICEDGE;
	AdjustWindowRectEx(&ClientRect, grfStyle, FALSE, grfExStyle);

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

	hWnd = CreateWindowEx(
		grfExStyle,
		MainClass.lpszClassName, std::string("SC2KRender - " + filename).c_str(),
		grfStyle,
		0, 0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, NULL, NULL, hInstance, NULL);

	RECT rc;
	GetClientRect(hWnd, &rc);
	
	scene = std::make_unique<Scene>();
	scene->Initialize(hWnd, tiles, rc);	
	ShowWindow(hWnd, TRUE);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (GetActiveWindow() == hWnd)
		{
			GetWindowRect(hWnd, &WindowRect);
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
	case WM_COMMAND:
		break;
	case WM_ERASEBKGND:
		break;
	case WM_PAINT:
		scene->Tick();
		break;
	case WM_MOUSEMOVE:
		POINT p;
		GetCursorPos(&p);
		scene->MouseLook(p.x, 0, p.y);
		break;
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		if (wParam == MK_LBUTTON)
			scene->MouseClick();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}