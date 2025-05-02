#pragma once
#include "wrl/client.h"
#include "d3d12.h"
#include <dxgi1_6.h>

namespace HEngine
{
	class DX12Resource : public Microsoft::WRL::ComPtr<ID3D12Resource>
	{
	public:
		DX12Resource() = default;
		~DX12Resource();

		void Initialize(ID3D12Device* pDevice, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, const UINT numAllocatedBytes);
		void InitializeAsDepthBuffer(ID3D12Device* pDevice, const UINT width, const UINT height);
		void Release();

	public:
		void* GetCPUMemory();

	private:
		void* mMemory = nullptr;
	};
}


