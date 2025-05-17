#pragma once
#include "Windows.h"
#include "IRenderer.h"
#include "DirectXMath.h"
#include "../Config/EngineDLLConfig.h"
#include "./RenderTypes/RenderDataTypes.h"
#include "DirectX12/Device/DXDevice.h"

#include "DirectX12/DXGI/DXGISwapChain.h"

#include "DirectX12/Commands/DXCommandQueue.h"
#include "DirectX12/Commands/DXCommandList.h"

#include "DirectX12/Resource/DX12Resource.h"

#include "DirectX12/Pipeline/DX12PipelineState.h"

#include "DirectX12/Descriptors/DX12DescriptorHeap.h"

#include "DirectX12/MemoryManagement/BufferUploader.h"

namespace HEngine
{
	class DX12Renderer : public IRenderer
	{
	public:
		DX12Renderer() = default;
		DX12Renderer(const DX12Renderer&) = delete;
		DX12Renderer& operator=(const DX12Renderer&) = delete;
		~DX12Renderer() override;

		void Initialize(HWND hwnd, const UINT width, const UINT height, Camera* pCamera) override;
		void Update(const float deltaTime) override;
		void Release() override;

	private:
		DXDevice mDevice;
		DXCommandQueue mCommandQueue;
		DXCommandList mCommandList;

		DXGISwapChain mSwapChain;

		DX12PipelineState mPipelineState;
		DX12Resource mDepthBuffer;

		DX12Resource mVertexBuffer;
		DX12Resource mIndexBuffer;
		DX12Resource mMaterialBuffer1;
		DX12Resource mConstantBufferPassData;

		DX12DescriptorHeap mDepthDescHeap;

		BufferUploader mBufferUploader;
	private:
		D3D12_VERTEX_BUFFER_VIEW mVBView;
		D3D12_INDEX_BUFFER_VIEW mIBView;

	private:
		D3D12_VIEWPORT mViewport;
		D3D12_RECT mSRRect;

	private:
		DirectX::XMMATRIX mViewProjectionMatrix;

	private:
		UINT mWidth = 0;
		UINT mHeight = 0;
	};
}


