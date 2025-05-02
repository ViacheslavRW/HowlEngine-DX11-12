#pragma once
#include <memory>
#include "./Config/EngineDLLConfig.h"
#include "./Renderer/IRenderer.h"
#include "./Renderer/RenderTypes/GraphicsAPI.h"
#include "./Systems/Timer.h"

namespace HEngine
{
	extern Timer gTimer;

	class __declspec(dllexport) Launch
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

	public:
		HWND WndHandle = nullptr;   

	private:
		UINT mMainWindowWidth = 800;
		UINT mMainWindowHeight = 600;
		// time and frames
		void CalculateFrameStats() const;
	private:
		std::unique_ptr<IRenderer> SetRenderer(GraphicsAPI api);
	};
}


