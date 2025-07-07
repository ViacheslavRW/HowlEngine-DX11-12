#pragma once
#include "../Config/EngineDLLConfig.h"
#include "IRenderer.h"
#include "Windows.h"
#include "DirectX11/DebugInfoManager.h"
#include "./RenderTypes/RenderDataTypes.h"
#include "./DirectX11/TextureManager.h"
#include "../Mesh/CubeMesh.h"
#include "../Mesh/PlaneMesh.h"
#include "../Mesh/Mesh.h"
#include "../Mesh/MeshLoader.h"
#include "../Camera/Camera.h"
#include "../Graphics/LightHelper.h"

namespace HEngine
{
	using namespace Microsoft::WRL;
	using namespace DirectX;

	class HE_API DX11Renderer : public IRenderer
	{
	public:
		DX11Renderer() = default;
		DX11Renderer(const DX11Renderer&) = delete;
		DX11Renderer* operator=(const DX11Renderer&) = delete;
		~DX11Renderer() override;

		void Initialize(HWND hwnd, const UINT width, const UINT height, Camera* pCamera) override;
		void Update(const float deltaTime) override;
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
		// light
		LightHelper mLightHelper;
	private:
		// viewport
		D3D11_VIEWPORT mViewPort = {};
		// camera
		Camera* pCamera;

	private:
		// textures
		ComPtr<ID3D11SamplerState> mSamplerState = nullptr;
		TextureManager mTextureManager = {};
	private:
		// Mesh
		MeshLoader mMeshLoader = {};

		std::unique_ptr<Mesh> mesh1;
		std::unique_ptr<Mesh> mesh2;

		std::unique_ptr<CubeMeshT> cube1;

		std::unique_ptr<PlaneMeshT> plane1;

	private:
		// Debug
#if defined(DEBUG) || defined(_DEBUG)
		DebugInfoManager mDebugInfoManager;
#endif
	};
}


