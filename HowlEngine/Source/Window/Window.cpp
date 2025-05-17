#include "pch.h"
#include "Window.h"
#include "../Launch.h"
#include "../Renderer/DirectX12/Debug/DXGIDebug.h"

namespace HEngine
{
	// window procedure function
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		switch(uMsg)
		{
			case WM_NCCREATE:
			{
				LPCREATESTRUCT lPar = reinterpret_cast<LPCREATESTRUCT>(lParam);
				Window* pWnd = reinterpret_cast<Window*>(lPar->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
				return true;
			} break;
			case WM_CREATE :
			{
				pWnd->OnWindowCreate(hwnd);
				
			} break;
			case WM_DESTROY:
			{
				pWnd->OnWindowDestroy();
				PostQuitMessage(0);
			} break;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	Window::Window(const int width, const int height, Launch* pLaunch)
		: mLaunchPtr(pLaunch), mLastWidth(width), mLastHeight(height)
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
		//SetFullScreen();
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

	void Window::SetFullScreen()
	{
		if (!isFullScreen)
		{
			HMONITOR monitor = MonitorFromWindow(WndHandle, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFO monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(monitor, &monitorInfo);
			RECT rcMonitor = monitorInfo.rcMonitor;

			SetWindowLongPtr(WndHandle, GWL_STYLE, WS_POPUP);
			SetWindowPos(WndHandle, HWND_TOP,
				rcMonitor.left, rcMonitor.top,
				rcMonitor.right - rcMonitor.left,
				rcMonitor.bottom - rcMonitor.top,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW);

			isFullScreen = true;
			isWindowed = false;
		}
	}

	void Window::SetWindowed()
	{
		if (!isWindowed)
		{
			SetWindowLongPtr(WndHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(WndHandle, HWND_TOP,
				250, 250, mLastWidth, mLastHeight,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW);

			isWindowed = true;
			isFullScreen = false;
		}
	}
}
