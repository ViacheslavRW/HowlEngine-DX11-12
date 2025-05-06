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

		static void InitializeDepthStencilView(
			ComPtr<ID3D11DepthStencilState>& pDepthStencilState,
			ComPtr<ID3D11Texture2D>& pDepthStencilTexture,
			ComPtr<ID3D11DepthStencilView>& pDepthStencilView,
			ComPtr<ID3D11Device> pDevice,
			const UINT width, 
			const UINT height
		);
	};
}


