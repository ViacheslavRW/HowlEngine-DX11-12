#include "pch.h"
#include "DXCommandList.h"

namespace HEngine
{
	DXCommandList::~DXCommandList()
	{
		Release();
	}

	void DXCommandList::Initialize(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE cmdType, ID3D12PipelineState* initPipelineState)
	{
		HRESULT aRes = pDevice->CreateCommandAllocator(cmdType, IID_PPV_ARGS(mCommandAllocator.GetAddressOf()));
		if (FAILED(aRes)) std::cout << "FAILED_TO_CREATE_COMMAND_ALLOCATOR" << std::endl;

		HRESULT lRes = pDevice->CreateCommandList(0, cmdType, mCommandAllocator.Get(), initPipelineState, IID_PPV_ARGS(GetAddressOf()));
		if (FAILED(lRes)) std::cout << "FAILED_TO_CREATE_COMMAND_LIST" << std::endl;

		Get()->QueryInterface(IID_PPV_ARGS(&mGFXCommandList));
		//this->GetGFXCommandList()->Close();
	}

	void DXCommandList::ResetCommandList()
	{
		mCommandAllocator.Get()->Reset();
		GetGFXCommandList()->Reset(mCommandAllocator.Get(), nullptr);
	}

	void DXCommandList::Release()
	{
		if (Get() && mGFXCommandList)
		{
			mGFXCommandList->Release();
		}

		if (Get())
		{
			Reset();
		}

		if (mCommandAllocator.Get())
		{
			mCommandAllocator.Reset();
		}
	}
}