#include <d2d1_1.h>
#include <vector>
#include <Dwmapi.h>
#include <ctime>
#include <thread>
#include <memory>
#include <fcntl.h>
#include <io.h>
#include <cstdio>
#include <iostream>
#include "MapLoader.h"

#include <dwrite.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "dwrite")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hWnd;
ID2D1Factory* factory = NULL;
IDWriteFactory* wfactory = NULL;
ID2D1HwndRenderTarget* target = NULL;
ID2D1SolidColorBrush* brush = NULL;
ID2D1SolidColorBrush* brush2 = NULL;
ID2D1SolidColorBrush* GrayBrush = NULL;

ID2D1SolidColorBrush* OPAQUE_RED_BRUSH = NULL;
ID2D1SolidColorBrush* OPAQUE_GREEN_BRUSH = NULL;
ID2D1SolidColorBrush* OPAQUE_ORANGE_BRUSH = NULL;

IDWriteTextFormat* format = NULL;

ID2D1Bitmap* m_pBitmap;
RECT WindowRect;

int width = 512;
int height = 512;
DWORD dwProcID = NULL;
DWORD dwActiveProcID = NULL;

POINT MousePoint;
BYTE VK_MOUSELEFT = 0x01;
BYTE VK_MOUSERIGHT = 0x02;
bool CanClick = true;

RECT ClientRect = {0, 0, 512, 512};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	AllocConsole();
	FILE* p_file;
	freopen_s(&p_file, "CONIN$", "r", stdin);
	freopen_s(&p_file, "CONOUT$", "w", stdout);
	freopen_s(&p_file, "CONOUT$", "w", stderr);
	std::cout << "SC2KRender" << std::endl;
	std::cout << "Created by Aleksander Krimsky" << std::endl;
	std::string filename;
	std::cout << "Enter a SC2K Map to load: ";
	std::cin >> filename;
	if (!filename.empty())
	{
		MapLoader::LoadMap(filename);
	}
	//Compensate client area for window styles
	DWORD grfStyle, grfExStyle;
	grfStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	grfExStyle = WS_EX_STATICEDGE;
	AdjustWindowRectEx(&ClientRect, grfStyle, FALSE, grfExStyle);

	WNDCLASSEX MainClass = {};
	MainClass.cbClsExtra = NULL;
	MainClass.cbWndExtra = NULL;
	MainClass.lpszMenuName = NULL;
	MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	MainClass.hbrBackground = CreateSolidBrush(NULL);
	MainClass.cbSize = sizeof(WNDCLASSEX);
	//MainClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(108));
	//MainClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(108));
	MainClass.hInstance = hInstance;
	MainClass.lpfnWndProc = WndProc;
	MainClass.lpszClassName = "WindowClassName";
	MainClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&MainClass);

	hWnd = CreateWindowEx(
		grfExStyle,
		MainClass.lpszClassName, "WindowClass",
		grfStyle,
		0, 0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, NULL, NULL, hInstance, NULL);

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&wfactory));
	wfactory->CreateTextFormat(L"Lucida Console", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-us", &format);

	GetWindowThreadProcessId(hWnd, &dwProcID);
	RECT rc;
	GetClientRect(hWnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(
		rc.right - rc.left,
		rc.bottom - rc.top
	);

	D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
	factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE),
		D2D1::HwndRenderTargetProperties(hWnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&target);

	target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);

	target->CreateSolidColorBrush(D2D1::ColorF(0x9ACD32, 1.0f), &brush);
	target->CreateSolidColorBrush(D2D1::ColorF(0xFF0000, 1.0f), &brush2);
	target->CreateSolidColorBrush(D2D1::ColorF(0x9A9FA8, 1.0f), &GrayBrush);

	target->CreateSolidColorBrush(D2D1::ColorF(0x9ACD32, 0.5f), &OPAQUE_GREEN_BRUSH);
	target->CreateSolidColorBrush(D2D1::ColorF(0xFF0000, 0.5f), &OPAQUE_RED_BRUSH);
	target->CreateSolidColorBrush(D2D1::ColorF(0xE5B329, 0.5f), &OPAQUE_ORANGE_BRUSH);
	ShowWindow(hWnd, FALSE);

	MSG msg;
	clock_t KeyTimer = clock();
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		GetWindowThreadProcessId(GetForegroundWindow(), &dwActiveProcID);
		if (dwActiveProcID == dwProcID)
		{
			GetWindowRect(hWnd, &WindowRect);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		Sleep(5);
	}
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
	}
	break;
	case WM_ERASEBKGND:
		break;
	case WM_PAINT:
	{
		target->BeginDraw();
		target->SetTransform(D2D1::Matrix3x2F::Identity());
		target->Clear();
		GetCursorPos(&MousePoint);
		ScreenToClient(hWnd, &MousePoint);		
		std::wstring text = L"Hello World";
		target->DrawText(text.c_str(), text.length(), format, D2D1::RectF(25, 25, 300, 20), brush);
		target->EndDraw();
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