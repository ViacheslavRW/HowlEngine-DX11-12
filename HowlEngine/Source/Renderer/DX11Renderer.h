#pragma once
#include "../Config/EngineDLLConfig.h"
#include "IRenderer.h"
#include "Windows.h"
#include "DirectX11/DebugInfoManager.h"
#include "./RenderTypes/RenderDataTypes.h"
#include "../Systems/ShaderCompiler.h"
#include "../Common/TextureManager.h"
#include "../Mesh/CubeMesh.h"
#include "../Mesh/PlaneMesh.h"
#include "../Mesh/Mesh.h"
#include "../Mesh/MeshLoader.h"
#include "../Mesh/MeshManager.h"
#include "../Mesh/MaterialManager.h"
#include "../Common/Camera.h"
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
		void Update(const float& deltaTime) override;
		void Release() override;
	private:
		// Main Components
		ComPtr<ID3D11Device> mDevice = nullptr;
		ComPtr<IDXGISwapChain> mSwapChain = nullptr;
		ComPtr<ID3D11DeviceContext> mDeviceContext = nullptr;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView = nullptr;
		// depth stencil
		ComPtr<ID3D11DepthStencilState> mDepthStencilState = nullptr;
		ComPtr<ID3D11DepthStencilState> mDepthStencilStateTransparent = nullptr;
		ComPtr<ID3D11Texture2D> mDepthStencilTexture = nullptr;
		ComPtr<ID3D11DepthStencilView> mDepthStencilView = nullptr;
		ComPtr<ID3D11RasterizerState> mRasterizer = nullptr;
		// input layout
		ComPtr<ID3D11InputLayout> mInputLayout = nullptr;
		ComPtr<ID3D11InputLayout> mInputLayoutPBR = nullptr;
		// shaders
		ShaderCompiler mShaderCompiler;
		// blend
		ComPtr<ID3D11BlendState> mBlendState;
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
		MaterialManager mMaterialManager;
		MeshLoader mMeshLoader;
		MeshManager mMeshManager;
	private:
		// Debug
#if defined(DEBUG) || defined(_DEBUG)
		DebugInfoManager mDebugInfoManager;
#endif
		HRESULT hRes;
	};
}


