#pragma once
#include "../../../Config/EngineGlobalSettings.h"
#include "wrl/client.h"
#include "d3d12.h"
#include <dxgi1_6.h>
#include "../Descriptors/DX12DescriptorHeap.h"

namespace HEngine
{
	class DXGISwapChain : public Microsoft::WRL::ComPtr<IDXGISwapChain1>
	{
	public:
		DXGISwapChain() = default;
		~DXGISwapChain();

		void Initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, const UINT width, const UINT height);

		void CreateBuffers();
		void DropBuffers();
		void Present();
		void Release();

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle();
	public:
		inline ID3D12Resource* GetCurrentRenderTarget() { return mRenderTargets[mCurrentBufferCount].Get(); };
		
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTargets[MAX_SWAPCHAIN_BUFFERS];
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRTVHeap;
		DX12DescriptorHeap mRTVHeap;

	private:
		ID3D12Device* mDevice = nullptr;

	private:
		USHORT mBufferCount = 0;
		USHORT mCurrentBufferCount = 0;

		UINT mWidth = 0;
		UINT mHeight = 0;

		UINT mHeapIncrement = 0;
	};
}


