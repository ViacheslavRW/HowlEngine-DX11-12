#pragma once

namespace HEngine
{
	using namespace Microsoft::WRL;

	enum class InputLayoutType
	{
		PBR,
		Skybox,
		Universal,
		Glass
	};

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

		static void SetViewPort(
			const UINT width,
			const UINT height,
			ComPtr<ID3D11DeviceContext>& pDeviceContext,
			D3D11_VIEWPORT& pViewPort
		);

		static void InitializeDepthStencilView(
			ComPtr<ID3D11DepthStencilState>& pDepthStencilState,
			ComPtr<ID3D11Texture2D>& pDepthStencilTexture,
			ComPtr<ID3D11DepthStencilView>& pDepthStencilView,
			ComPtr<ID3D11Device> pDevice,
			const UINT width, 
			const UINT height
		);

		static void InitializeDepthStencilViewTransparent(
			ComPtr<ID3D11DepthStencilState>& pDepthStencilState,
			ComPtr<ID3D11Device> pDevice
		);

		static void InitializeDepthStencilViewSkybox(
			ComPtr<ID3D11DepthStencilState>& pDepthStencilState,
			ComPtr<ID3D11Device> pDevice
		);

		static void InitializeRasterizer(
			ComPtr<ID3D11Device>& pDevice,
			ComPtr<ID3D11DeviceContext>& pDeviceContext,
			ComPtr<ID3D11RasterizerState>& pRasterizer
		);

		static void InitializeInputLayout(
			InputLayoutType type,
			ComPtr<ID3DBlob>& pBlob,
			ComPtr<ID3D11InputLayout>& pInputLayout,
			ComPtr<ID3D11Device>& pDevice
		);

		static void InitializeBlendState(
			ComPtr<ID3D11Device>& pDevice,
			ComPtr<ID3D11BlendState>& pBlendState
		);

		static void InitializeTextureSampler(
			ComPtr<ID3D11Device>& pDevice,
			ComPtr<ID3D11DeviceContext>& pDeviceContext,
			ComPtr<ID3D11SamplerState>& pSampleState
		);
	};
}


