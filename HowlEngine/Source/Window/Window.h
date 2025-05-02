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

	private:
		Launch* mLaunchPtr = nullptr;
	};
}


