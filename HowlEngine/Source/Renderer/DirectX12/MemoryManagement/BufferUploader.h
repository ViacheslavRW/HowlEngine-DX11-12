#pragma once
#include "../Resource/DX12Resource.h"
#include "../Commands/DXCommandList.h"
#include "../Commands/DXCommandQueue.h"

namespace HEngine
{
	class BufferUploader
	{
	public:
		BufferUploader() = default;
		~BufferUploader();

		void Initialize(ID3D12Device* pDevice, const UINT memoryAllocation);

		void Upload(DX12Resource* pDestination, void* pSourceData, const UINT size, 
			DXCommandList* pCommandList, DXCommandQueue* pCommandQueue, D3D12_RESOURCE_STATES customState = D3D12_RESOURCE_STATE_COMMON);

		void Release();

	private:
		DX12Resource mInternalBuffer;

		void* mInternalMemory = nullptr;

		UINT mMaxSize = 0;
	};
}


