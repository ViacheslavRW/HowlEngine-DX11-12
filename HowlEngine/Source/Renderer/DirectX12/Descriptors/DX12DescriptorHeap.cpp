#include "pch.h"
#include "DX12DescriptorHeap.h"

namespace HEngine
{
	DX12DescriptorHeap::~DX12DescriptorHeap()
	{
		Release();
	}

	void DX12DescriptorHeap::Initialize(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool isShaderVisible)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = heapType;
		heapDesc.NumDescriptors = numDescriptors;
		heapDesc.NodeMask = 0;
		heapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT res = pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(GetAddressOf()));
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE: " << heapType << std::endl;

		mDescSize = pDevice->GetDescriptorHandleIncrementSize(heapType);
	}

	void DX12DescriptorHeap::Release()
	{
		if (Get())
		{
			Reset();
		}
	}
}