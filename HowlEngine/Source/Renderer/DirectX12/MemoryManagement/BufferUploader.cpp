#include "pch.h"
#include "BufferUploader.h"

namespace HEngine
{
	BufferUploader::~BufferUploader()
	{
		Release();
	}

	void BufferUploader::Initialize(ID3D12Device* pDevice, const UINT memoryAllocation)
	{
		mInternalBuffer.Initialize(pDevice, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, memoryAllocation);

		mInternalMemory = mInternalBuffer.GetCPUMemory();
		mMaxSize = memoryAllocation;
	}

	void BufferUploader::Upload(DX12Resource* pDestination, void* pSourceData, const UINT size, DXCommandList* pCommandList, DXCommandQueue* pCommandQueue, D3D12_RESOURCE_STATES customState)
	{
		memcpy(mInternalMemory, pSourceData, size);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pDestination->Get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

		pCommandList->GetGFXCommandList()->ResourceBarrier(1, &barrier);
		pCommandList->GetGFXCommandList()->CopyBufferRegion(pDestination->Get(), 0, mInternalBuffer.Get(), 0 , size);

		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pDestination->Get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = (customState == D3D12_RESOURCE_STATE_COMMON ? D3D12_RESOURCE_STATE_COMMON : customState);

		pCommandList->GetGFXCommandList()->ResourceBarrier(1, &barrier);
		pCommandList->GetGFXCommandList()->Close();

		pCommandQueue->_ExecuteCommandList(pCommandList->Get());

		while (pCommandQueue->GetFence()->GetCompletedValue() < pCommandQueue->_GetCurrentFenceValue())
		{
			_mm_pause();
		}

		pCommandList->ResetCommandList();
	}

	void BufferUploader::Release()
	{
		if (mInternalMemory)
		{
			mInternalMemory = nullptr;
		}
		mInternalBuffer.Release();
	}
}
