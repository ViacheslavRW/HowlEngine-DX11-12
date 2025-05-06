#pragma once
#include "Windows.h"
#include "IRenderer.h"
#include "DirectX11/DebugInfoManager.h"
#include "./RenderTypes/RenderDataTypes.h"

namespace HEngine
{
	using namespace Microsoft::WRL;
	using namespace DirectX;

	class DX11Renderer : public IRenderer
	{
	public:
		DX11Renderer() = default;
		DX11Renderer(const DX11Renderer&) = delete;
		DX11Renderer* operator=(const DX11Renderer&) = delete;
		~DX11Renderer() override;

		void Initialize(HWND hwnd, const UINT width, const UINT height) override;
		void Update(const double deltaTime) override;
		void Release() override;

	private:
		// Main Components
		ComPtr<ID3D11Device> mDevice = nullptr;
		ComPtr<IDXGISwapChain> mSwapChain = nullptr;
		ComPtr<ID3D11DeviceContext> mDeviceContext = nullptr;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView = nullptr;
		// depth stencil
		ComPtr<ID3D11DepthStencilState> mDepthStencilState = nullptr;
		ComPtr<ID3D11Texture2D> mDepthStencilTexture = nullptr;
		ComPtr<ID3D11DepthStencilView> mDepthStencilView = nullptr;
	private:
		// viewport
		D3D11_VIEWPORT mViewPort = {};
		XMMATRIX mViewProjectionMatrix = {};

	private:
		// Debug
#if defined(DEBUG) || defined(_DEBUG)
		DebugInfoManager mDebugInfoManager;
#endif

	private:
		// Test
		float mAngle = 45.0f;
		
		ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mConstantBuffer = nullptr;
		ComPtr<ID3D11InputLayout> mInputLayout = nullptr;
		ComPtr<ID3D11VertexShader> mVertexShader;
		ComPtr<ID3D11PixelShader> mPixelShader;

		void InitCube(TR::Vertex3D vertices[], unsigned short indices[]);
		void DrawCube(const double deltaTime, UINT indices, float angle);
		UINT size = 36;
	};
}


