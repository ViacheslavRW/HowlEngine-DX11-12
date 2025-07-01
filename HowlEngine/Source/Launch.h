#pragma once
#include <memory>
#include "./Camera/Camera.h"
#include "Windows.h"
#include "./Config/EngineDLLConfig.h"
#include "./Renderer/IRenderer.h"
#include "./Renderer/RenderTypes/GraphicsAPI.h"
#include "./Systems/Timer.h"
#include "./Input/InputManager.h"

namespace HEngine
{
	extern Timer gTimer;

	class HE_API Launch
	{
	public:
		// Main Application Loop
	    void AppRun();
		void AppUpdate();
		void AppExit();

		// Application Status
		bool isAppInitialized = false;
		bool isAppRunning = false;

	public:
		std::unique_ptr<IRenderer> mRenderer;
		Camera mCamera;
		InputManager mInputManager;
	public:
		HWND WndHandle = nullptr;   
	private:
		UINT mMainWindowWidth = 1280;
		UINT mMainWindowHeight = 720;
		// time and frames
		void CalculateFrameStats() const;
		
	private:
		std::unique_ptr<IRenderer> SetRenderer(GraphicsAPI api);
	};
}


