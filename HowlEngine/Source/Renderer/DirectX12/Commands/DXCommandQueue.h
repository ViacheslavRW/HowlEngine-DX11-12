#pragma once
#include "wrl/client.h"

namespace HEngine
{
	class DXCommandQueue : public Microsoft::WRL::ComPtr<ID3D12CommandQueue>
	{
	public:
		DXCommandQueue() = default;
		~DXCommandQueue();

		void Initialize(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE cmdType);

		void Release();
		void FlushQueue();

	public:
		void _ExecuteCommandList(ID3D12CommandList* pCommandList);
		void WaitForGPU();
	public:
		inline ID3D12Fence* GetFence() { return mFence.Get(); };
		inline UINT64 _GetCurrentFenceValue() const { return mCurrentFenceValue; };

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

		UINT64 mCurrentFenceValue = 0;
	};
}


