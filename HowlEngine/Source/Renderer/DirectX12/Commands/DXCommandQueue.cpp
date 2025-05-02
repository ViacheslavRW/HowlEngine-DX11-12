#include "pch.h"
#include "DXCommandQueue.h"


namespace HEngine
{
    DXCommandQueue::~DXCommandQueue()
    {
        Release();
    }

    void DXCommandQueue::Initialize(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE cmdType)
    {
        // may be implemented multiple command queues in the future
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = cmdType;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        HRESULT qRes = pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(GetAddressOf()));
        if (FAILED(qRes)) std::cout << "FAILED_TO_CREATE_COMMAND_QUEUE" << std::endl;

        HRESULT fRes = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));
        if (FAILED(fRes)) std::cout << "FAILED_TO_CREATE_FENCE" << std::endl;
    }

    void DXCommandQueue::Release()
    {
        if (Get())
        {
            Reset();
        }

        if (mFence.Get())
        {
            mFence.Reset();
        }
    }

    void DXCommandQueue::FlushQueue()
    {
        if (Get()) 
        {
            for (UINT i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++)
            {
                Get()->Signal(mFence.Get(), mCurrentFenceValue + i);
            }
        }
    }

    void DXCommandQueue::_ExecuteCommandList(ID3D12CommandList* pCommandList)
    {
        Get()->ExecuteCommandLists(1, &pCommandList);
        mCurrentFenceValue++;
        Get()->Signal(mFence.Get(), mCurrentFenceValue);
    }

    void DXCommandQueue::WaitForGPU()
    {
        if (mFence->GetCompletedValue() < mCurrentFenceValue)
        {
            HANDLE eventHandle = CreateEvent(nullptr, false, false, nullptr);
            if (eventHandle != nullptr)
            {
                mFence->SetEventOnCompletion(mCurrentFenceValue, eventHandle);
                WaitForSingleObject(eventHandle, INFINITE);
                CloseHandle(eventHandle);
            }
            else
            {
                // in case of event creation error
                while (mFence->GetCompletedValue() < mCurrentFenceValue)
                {
                    _mm_pause();
                }
            }
        }
    }
}