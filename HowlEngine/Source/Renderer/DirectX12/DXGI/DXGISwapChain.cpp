#include "pch.h"
#include "DXGISwapChain.h"


namespace HEngine
{
	DXGISwapChain::~DXGISwapChain()
	{
		Release();
	}

	void DXGISwapChain::Initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, const UINT width, const UINT height)
	{
		// Copy variables
		mWidth = width;
		mHeight = height;

		mRTVHeap.Initialize(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, MAX_SWAPCHAIN_BUFFERS, false);
		mHeapIncrement = mRTVHeap.GetDescriptorSize();

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.Stereo = false;
		desc.SampleDesc = { 1, 0 };
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = MAX_SWAPCHAIN_BUFFERS;
		desc.Scaling = DXGI_SCALING_NONE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT sRes = pFactory->CreateSwapChainForHwnd(pCommandQueue, hwnd, &desc, nullptr, nullptr, GetAddressOf());
		if (FAILED(sRes)) std::cout << "FAILED_TO_CREATE_SWAPCHAIN" << std::endl;

		// store some values
		mBufferCount = MAX_SWAPCHAIN_BUFFERS;
		mDevice = pDevice;

		CreateBuffers();
	}

	void DXGISwapChain::CreateBuffers()
	{
		if (!Get())
		{
			std::cout << "CANNOT_CREATE_BUFFERS_FOR_UNINITIALIZED_SWAPCHAIN" << std::endl;
			return;
		}

		for (int i = 0; i < mBufferCount; i++)
		{
			mRenderTargets[i].Reset();
			HRESULT bRes = Get()->GetBuffer(i, IID_PPV_ARGS(mRenderTargets[i].GetAddressOf()));
			if (FAILED(bRes)) std::cout << "FAILED_TO_CREATE_BUFFER" << std::endl;

			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (SIZE_T)mHeapIncrement * i;

			mDevice->CreateRenderTargetView(mRenderTargets[i].Get(), nullptr, cpuHandle);
		}
	}

	void DXGISwapChain::DropBuffers()
	{
		for (int i = 0; i < mBufferCount; i++)
		{
			mRenderTargets[i].Reset();
			std::cout << "A_FRAME_WAS_RELEASED" << std::endl;
		}
	}

	void DXGISwapChain::Present()
	{
		Get()->Present(0, 0);
		mCurrentBufferCount = (mCurrentBufferCount + 1) % mBufferCount;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DXGISwapChain::GetCurrentRTVHandle()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += (SIZE_T)mCurrentBufferCount * mHeapIncrement;

		return cpuHandle;
	}

	void DXGISwapChain::Release()
	{
		if (Get())
		{
			DropBuffers();
			mRTVHeap.Reset();
			Reset();
		}
	}
}