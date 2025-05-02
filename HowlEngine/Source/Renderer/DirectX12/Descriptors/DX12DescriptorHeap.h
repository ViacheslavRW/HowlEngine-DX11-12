#pragma once
#include <wrl.h>

namespace HEngine
{
	class DX12DescriptorHeap : public Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
	{
	public:
		DX12DescriptorHeap() = default;
		~DX12DescriptorHeap();

		void Initialize(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool isShaderVisible);

		void Release();

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStart();

	public:
		inline UINT GetDescriptorSize() const { return mDescSize; };
	private:
		UINT mDescSize = 0;
		UINT mCurrentOffset = 0;
	};
}


