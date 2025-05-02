#include "pch.h"
#include "DX12Resource.h"

namespace HEngine
{
	DX12Resource::~DX12Resource()
	{
		Release();
	}

	void DX12Resource::Initialize(ID3D12Device* pDevice, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, const UINT numAllocatedBytes)
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = heapType;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;
		
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = numAllocatedBytes;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc = { 1, 0 };
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT res = pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, initialState, nullptr, IID_PPV_ARGS(GetAddressOf()));
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_COMMITED_RESOURCE" << std::endl;
	}

	void DX12Resource::InitializeAsDepthBuffer(ID3D12Device* pDevice, const UINT width, const UINT height)
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Alignment = 0;
		resDesc.Width = width;
		resDesc.Height = height;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1; // was 0
		resDesc.Format = DXGI_FORMAT_D32_FLOAT;
		resDesc.SampleDesc = { 1, 0 };
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0.0f;

		HRESULT res = pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(GetAddressOf()));
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_DEPTH_BUFFER" << std::endl;
	}

	void DX12Resource::Release()
	{
		if (mMemory && Get())
		{
			Get()->Unmap(0, 0);
		}

		if (Get())
		{
			Reset();
		}
	}

	void* DX12Resource::GetCPUMemory()
	{
		if (!mMemory)
		{
			Get()->Map(0, 0, &mMemory);
		}
		return mMemory;
	}
}