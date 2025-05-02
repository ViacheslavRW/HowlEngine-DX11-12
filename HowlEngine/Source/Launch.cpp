#include "pch.h"
#include "Launch.h"
#include "Window/Window.h"
#include "Renderer/DX12Renderer.h"
#include "Renderer/DX11Renderer.h"

namespace HEngine
{
	Timer gTimer;

	void Launch::AppRun()
	{
		Window mainWindow(mMainWindowWidth, mMainWindowHeight, this);

		mRenderer = SetRenderer(GraphicsAPI::DirectX11);
		mRenderer->Initialize(WndHandle, mMainWindowWidth, mMainWindowHeight);

		gTimer.Reset();

		isAppInitialized = true;
		isAppRunning = true;
	}

	void Launch::AppUpdate()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			if (msg.message == WM_QUIT) isAppRunning = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		mRenderer->Update();
		gTimer.Tick();
		CalculateFrameStats();
	}

	void Launch::AppExit()
	{
		//if(mRenderer) mRenderer->Release();
	}

	void Launch::CalculateFrameStats() const
	{
		static int frameCount = 0;
		static float timeElapsed = 0.0f;

		frameCount++;
		timeElapsed += gTimer.GetDeltaTime();

		if (timeElapsed >= 1.0f)
		{
			float fps = (float)frameCount;
			float mspf = 1000.0f / fps;

			std::wstring title = L"Howl Engine | FPS: " + std::to_wstring(static_cast<int>(fps)) +
				L" | Frametime: " + std::to_wstring(mspf).substr(0, 5) + L"ms";

			SetWindowText(WndHandle, title.c_str());

			frameCount = 0;
			timeElapsed = 0.0f;
		}
	}

	std::unique_ptr<IRenderer> Launch::SetRenderer(GraphicsAPI api)
	{
		switch (api)
		{
			case GraphicsAPI::DirectX12: 
				return std::make_unique<DX12Renderer>();
			case GraphicsAPI::DirectX11:
				return std::make_unique<DX11Renderer>();
			default: return nullptr;
		}
	}
}