#pragma once

#include "Windows.h"
#include "../Launch.h"

namespace HEngine
{
	class Window
	{
	public:
		Window(const int width, const int height, Launch* pLaunch);
		~Window();

		void OnWindowCreate(HWND hWnd);
		void OnWindowDestroy();

		HWND WndHandle = nullptr;
		bool isWindowRunning = false;

		// toggle windowed and fullscreen
		void SetFullScreen();
		void SetWindowed();
		bool isFullScreen = false;
		bool isWindowed = true;
	private:
		Launch* mLaunchPtr = nullptr;
		UINT mLastWidth = 0;
		UINT mLastHeight = 0;
	};
}


