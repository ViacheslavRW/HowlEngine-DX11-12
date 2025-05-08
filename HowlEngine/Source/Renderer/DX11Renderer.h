#pragma once
#include "Windows.h"
#include "IRenderer.h"
#include "DirectX11/DebugInfoManager.h"
#include "./RenderTypes/RenderDataTypes.h"
#include "./DirectX11/TextureManager.h"
#include "../Mesh/CubeMesh.h"

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
		ComPtr<ID3D11RasterizerState> mRasterizer = nullptr;
		// input layout
		ComPtr<ID3D11InputLayout> mInputLayout = nullptr;
		// shaders
		ComPtr<ID3D11VertexShader> mVertexShader;
		ComPtr<ID3D11PixelShader> mPixelShader;
	private:
		// viewport
		D3D11_VIEWPORT mViewPort = {};
		XMMATRIX mViewMatrix = XMMatrixTranslation(0.0f, 0.0f, 4.5f);
		XMMATRIX mProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f / 0.75f, 0.1f, 10.0f);

	private:
		// textures
		ComPtr<ID3D11SamplerState> mSamplerState = nullptr;
		TextureManager mTextureManager = {};
	private:
		// Mesh
		std::unique_ptr<CubeMeshT> cube1;
		std::unique_ptr<CubeMeshT> cube2;
		std::unique_ptr<CubeMeshT> cube3;

		// Other
		float mAngle = 45.0f;

	private:
		// Debug
#if defined(DEBUG) || defined(_DEBUG)
		DebugInfoManager mDebugInfoManager;
#endif
	};
}


