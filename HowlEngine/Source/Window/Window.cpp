#include "pch.h"
#include "Window.h"
#include "../Launch.h"
#include "../Renderer/DirectX12/Debug/DXGIDebug.h"

namespace HEngine
{
	// window procedure function
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg)
		{
			case WM_NCCREATE:
			{
				LPCREATESTRUCT lPar = reinterpret_cast<LPCREATESTRUCT>(lParam);
				Window* pWnd = reinterpret_cast<Window*>(lPar->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

			} break;
			case WM_CREATE :
			{
				Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				pWnd->OnWindowCreate(hwnd);
				
			} break;
			case WM_DESTROY:
			{
				Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				pWnd->OnWindowDestroy();
				PostQuitMessage(0);
			} break;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	Window::Window(const int width, const int height, Launch* pLaunch)
		: mLaunchPtr(pLaunch)
	{
		WNDCLASS wc = {};
		wc.style = NULL;
		wc.lpfnWndProc = WindowProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wc.lpszClassName = L"Window Class";

		RegisterClass(&wc);

		WndHandle = CreateWindow(L"Window Class", L"Howl Engine", WS_OVERLAPPEDWINDOW, 
			250, 250, width, height,
			NULL, NULL, NULL, this
			);

		if (!WndHandle)
		{
			isWindowRunning = false;
			return;
		}

		ShowWindow(WndHandle, SW_SHOW);
		UpdateWindow(WndHandle);

		isWindowRunning = true;
	}

	Window::~Window()
	{
		isWindowRunning = false;
	}

	void Window::OnWindowCreate(HWND hWnd)
	{
		mLaunchPtr->WndHandle = hWnd;
	}

	void Window::OnWindowDestroy()
	{
		DXGIDebug::Get().GetLiveObjects();
	}
}
