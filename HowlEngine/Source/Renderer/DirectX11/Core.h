#pragma once

namespace HEngine
{
	using namespace Microsoft::WRL;

	class Core
	{
	public:
		Core() = default;
		Core(const Core&) = delete;
		Core* operator=(const Core&) = delete;

	public:
		static void InitializeDeviceAndSwapChain(
			HWND hwnd,
			const UINT width, 
			const UINT height,
			ComPtr<ID3D11Device>& pDevice,
			ComPtr<IDXGISwapChain>& pSwapChain,
			ComPtr<ID3D11DeviceContext>& pDeviceContext
		);

		static void InitializeRenderTargetView(
			ComPtr<ID3D11RenderTargetView>& pRTV,
			ComPtr<ID3D11Device>& pDevice,
			ComPtr<IDXGISwapChain>& pSwapChain
		);
	};
}


