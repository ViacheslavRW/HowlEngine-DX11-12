#include "pch.h"
#include "DX12Renderer.h"
#include "DirectX12/DXGI/DXGIFactory.h"
#include "DirectX12/DXGI/DXGIAdapter.h"
#include "DirectX12/Debug/DXDebug.h"

namespace HEngine
{
	DX12Renderer::~DX12Renderer()
	{
		Release();
	}

	void DX12Renderer::Initialize(HWND hwnd, const UINT width, const UINT height, Camera* pCamera)
	{
		// Setup variables
		mWidth = width;
		mHeight = height;

		// Enable Debugging
		DXDebug::Get().Enable();

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();
		mDevice.Initialize(adapter.Get());

		mCommandQueue.Initialize(mDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
		mCommandList.Initialize(mDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

		mSwapChain.Initialize(mDevice.Get(), factory.Get(), mCommandQueue.Get(), hwnd, width, height);

		mBufferUploader.Initialize(mDevice.Get(), 64*1024);

		mVertexBuffer.Initialize(mDevice.Get(), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, 8*1024);

		// --cube 2--
#define G_Indices 24
		TR::Vertex verticesCube[G_Indices];
		// positions
		verticesCube[0].position = { -1.0f, 1.0f, 1.0f };
		verticesCube[1].position = { -1.0f, 1.0f, -1.0f };
		verticesCube[2].position = { 1.0f, 1.0f, 1.0f };
		verticesCube[3].position = { -1.0f, -1.0f, -1.0f };
		verticesCube[4].position = { 1.0f, -1.0f, -1.0f };
		verticesCube[5].position = { -1.0f, 1.0f, -1.0f };
		verticesCube[6].position = { 1.0f, -1.0f, -1.0f };
		verticesCube[7].position = { 1.0f, -1.0f, 1.0f };
		verticesCube[8].position = { 1.0f, 1.0f, -1.0f };
		verticesCube[9].position = { 1.0f, -1.0f, 1.0f };
		verticesCube[10].position = { 1.0f, -1.0f, -1.0f };
		verticesCube[11].position = { -1.0f, -1.0f, 1.0f };
		verticesCube[12].position = { -1.0f, -1.0f, 1.0f };
		verticesCube[13].position = { -1.0f, -1.0f, -1.0f };
		verticesCube[14].position = { -1.0f, 1.0f, 1.0f };
		verticesCube[15].position = { 1.0f, -1.0f, 1.0f };
		verticesCube[16].position = { -1.0f, -1.0f, 1.0f };
		verticesCube[17].position = { 1.0f, 1.0f, 1.0f };
		verticesCube[18].position = {1.0f, 1.0f, -1.0f};
		verticesCube[19].position = { 1.0f, 1.0f, -1.0f };
		verticesCube[20].position = { 1.0f, 1.0f, 1.0f };
		verticesCube[21].position = { -1.0f, -1.0f, -1.0f };
		verticesCube[22].position = { -1.0f, 1.0f, -1.0f };
		verticesCube[23].position = { -1.0f, 1.0f, 1.0f };
		// normals
		verticesCube[0].normal = { 0.0f, 1.0f, 0.0f };
		verticesCube[1].normal = { 0.0f, 1.0f, 0.0f };
		verticesCube[2].normal = { 0.0f, 1.0f, 0.0f };
		verticesCube[3].normal = { 0.0f, 0.0f, -1.0f };
		verticesCube[4].normal = { 0.0f, 0.0f, -1.0f };
		verticesCube[5].normal = { 0.0f, 0.0f, -1.0f };
		verticesCube[6].normal = { 1.0f, 0.0f, 0.0f };
		verticesCube[7].normal = { 1.0f, 0.0f, 0.0f };
		verticesCube[8].normal = { 1.0f, 0.0f, 0.0f };
		verticesCube[9].normal = { 0.0f, -1.0f, 0.0f };
		verticesCube[10].normal = { 0.0f, -1.0f, 0.0f };
		verticesCube[11].normal = { 0.0f, -1.0f, 0.0f };
		verticesCube[12].normal = { -1.0f, 0.0f, 0.0f };
		verticesCube[13].normal = { -1.0f, 0.0f, 0.0f };
		verticesCube[14].normal = { -1.0f, 0.0f, 0.0f };
		verticesCube[15].normal = { 0.0f, 0.0f, 1.0f };
		verticesCube[16].normal = { 0.0f, 0.0f, 1.0f };
		verticesCube[17].normal = { 0.0f, 0.0f, 1.0f };
		verticesCube[18].normal = { 0.0f, 1.0f, 0.0f };
		verticesCube[19].normal = { 0.0f, 0.0f, -1.0f };
		verticesCube[20].normal = { 1.0f, 0.0f, 0.0f };
		verticesCube[21].normal = { 0.0f, -1.0f, 0.0f };
		verticesCube[22].normal = { -1.0f, 0.0f, 0.0f };
		verticesCube[23].normal = { 0.0f, 0.0f, 1.0f };

		mBufferUploader.Upload((DX12Resource*)mVertexBuffer.GetAddressOf(), verticesCube, sizeof(TR::Vertex) * G_Indices,
			(DXCommandList*)mCommandList.GetAddressOf(), (DXCommandQueue*)mCommandQueue.GetAddressOf());

		mVBView.BufferLocation = mVertexBuffer.Get()->GetGPUVirtualAddress();
		mVBView.StrideInBytes = sizeof(TR::Vertex);
		mVBView.SizeInBytes = 8 * 1024;

		UINT32 indices[36];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 3;
		indices[4] = 4;
		indices[5] = 5;

		indices[6] = 6;
		indices[7] = 7;
		indices[8] = 8;

		indices[9] = 9;
		indices[10] = 10;
		indices[11] = 11;

		indices[12] = 12;
		indices[13] = 13;
		indices[14] = 14;

		indices[15] = 15;
		indices[16] = 16;
		indices[17] = 17;

		indices[18] = 1;
		indices[19] = 18;
		indices[20] = 2;

		indices[21] = 4;
		indices[22] = 19;
		indices[23] = 5;

		indices[24] = 7;
		indices[25] = 20;
		indices[26] = 8;

		indices[27] = 10;
		indices[28] = 21;
		indices[29] = 11;

		indices[30] = 13;
		indices[31] = 22;
		indices[32] = 14;

		indices[33] = 16;
		indices[34] = 23;
		indices[35] = 17;

		mIndexBuffer.Initialize(mDevice.Get(), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, 8 * 1024);

		mBufferUploader.Upload((DX12Resource*)mIndexBuffer.GetAddressOf(), indices, sizeof(UINT32) * 36,
			(DXCommandList*)mCommandList.GetAddressOf(), (DXCommandQueue*)mCommandQueue.GetAddressOf());

		mIBView.BufferLocation = mIndexBuffer.Get()->GetGPUVirtualAddress();
		mIBView.Format = DXGI_FORMAT_R32_UINT;
		mIBView.SizeInBytes = 8 * 1024;

		mPipelineState.Initialize(mDevice.Get());

		// depth/stencil
		mDepthBuffer.InitializeAsDepthBuffer(mDevice.Get(), mWidth, mHeight);

		mDepthDescHeap.Initialize(mDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsvDesc, mDepthDescHeap.Get()->GetCPUDescriptorHandleForHeapStart());

		mViewport.TopLeftX = 0;
		mViewport.TopLeftY = 0;
		mViewport.Width = static_cast<float>(mWidth);
		mViewport.Height = static_cast<float>(mHeight);
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;

		mSRRect.left = 0;
		mSRRect.right = static_cast<LONG>(mViewport.Width);
		mSRRect.top = 0;
		mSRRect.bottom = static_cast<LONG>(mViewport.Height);

		// view matrix setup
		DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 1.0f);  
		DirectX::XMVECTOR cameraTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);   
		DirectX::XMVECTOR upDir = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPos, cameraTarget, upDir);

		// projection matrix setup
		DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(45.0f),
			static_cast<float>(mWidth)/ static_cast<float>(mHeight),
			1.0f,
			100.0f
		);

		mViewProjectionMatrix = viewMatrix * projectionMatrix;

		mConstantBufferPassData.Initialize(mDevice.Get(), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ,
			Utils::CalculateConstantBufferAlignment(sizeof(TR::PassData)));

		// material section
		mMaterialBuffer1.Initialize(mDevice.Get(), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, 
			Utils::CalculateConstantBufferAlignment(sizeof(TR::MaterialCelShader)));

		TR::MaterialCelShader material1;
		material1.diffuseAlbedo = { 0.8f, 0.1f, 0.1f, 1.0f };

		mBufferUploader.Upload((DX12Resource*)mMaterialBuffer1.GetAddressOf(), &material1, sizeof(TR::MaterialCelShader),
			(DXCommandList*)mCommandList.GetAddressOf(), (DXCommandQueue*)mCommandQueue.GetAddressOf());
	}

	void DX12Renderer::Update(const float& deltaTime)
	{
		memcpy(mConstantBufferPassData.GetCPUMemory(), &mViewProjectionMatrix, sizeof(TR::PassData));

		D3D12_RESOURCE_BARRIER barrier = {};

		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		mCommandList.GetGFXCommandList()->ResourceBarrier(1, &barrier);

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mSwapChain.GetCurrentRTVHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDepthDescHeap->GetCPUDescriptorHandleForHeapStart();

		mCommandList.GetGFXCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		mCommandList.GetGFXCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		mCommandList.GetGFXCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// binding
		mCommandList.GetGFXCommandList()->RSSetViewports(1, &mViewport);
		mCommandList.GetGFXCommandList()->RSSetScissorRects(1, &mSRRect);

		mCommandList.GetGFXCommandList()->SetGraphicsRootSignature(mPipelineState.GetRootSugnature());
		mCommandList.GetGFXCommandList()->SetPipelineState(mPipelineState.Get());
		mCommandList.GetGFXCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mCommandList.GetGFXCommandList()->IASetVertexBuffers(0, 1, &mVBView);
		mCommandList.GetGFXCommandList()->IASetIndexBuffer(&mIBView);

		mCommandList.GetGFXCommandList()->SetGraphicsRootConstantBufferView(0, mConstantBufferPassData.Get()->GetGPUVirtualAddress());
		mCommandList.GetGFXCommandList()->SetGraphicsRootConstantBufferView(1, mMaterialBuffer1.Get()->GetGPUVirtualAddress());

		// draw calls
		mCommandList.GetGFXCommandList()->DrawIndexedInstanced(36, 1, 0, 0, 0);
			
		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		mCommandList.GetGFXCommandList()->ResourceBarrier(1, &barrier);


		mCommandList.GetGFXCommandList()->Close();
		mCommandQueue._ExecuteCommandList(mCommandList.Get());

		mSwapChain.Present();

		mCommandQueue.WaitForGPU();

		mCommandList.ResetCommandList();
	}

	void DX12Renderer::Release()
	{
		mVertexBuffer.Release();

		mCommandQueue.FlushQueue();

		mSwapChain.Release();

		mCommandList.Release();
		mCommandQueue.Release();

		if (mDevice.Get()) mDevice.Reset();
	}
}

/*
		// --cube--
		TR::Vertex verticesCube[30];
		// front plane 1
		verticesCube[0].position = { -1.0f, -1.0f, -1.0f };
		verticesCube[1].position = { -1.0f, 1.0f, -1.0f };
		verticesCube[2].position = { 1.0f, -1.0f, -1.0f };
		// front plane 2
		verticesCube[3].position = { -1.0f, 1.0f, -1.0f };
		verticesCube[4].position = { 1.0f, 1.0f, -1.0f };
		verticesCube[5].position = { 1.0f, -1.0f, -1.0f };
		// back plane 1
		verticesCube[6].position = { -1.0f, -1.0f, 1.0f };
		verticesCube[7].position = { -1.0f, 1.0f, 1.0f };
		verticesCube[8].position = { 1.0f, 1.0f, 1.0f };
		// back plane 2
		verticesCube[9].position = { 1.0f, -1.0f, 1.0f };
		verticesCube[10].position = { -1.0f, -1.0f, 1.0f };
		verticesCube[11].position = { 1.0f, 1.0f, 1.0f };
		// left plane 1
		verticesCube[12].position = { -1.0f, -1.0f, 1.0f };
		verticesCube[13].position = { -1.0f, -1.0f, -1.0f };
		verticesCube[14].position = { -1.0f, 1.0f, -1.0f };
		// left plane 2
		verticesCube[15].position = { -1.0f, 1.0f, -1.0f };
		verticesCube[16].position = { -1.0f, 1.0f, 1.0f };
		verticesCube[17].position = { -1.0f, -1.0f, 1.0f };
		// right plane 1
		verticesCube[18].position = { 1.0f, -1.0f, -1.0f };
		verticesCube[19].position = { 1.0f, -1.0f, 1.0f };
		verticesCube[20].position = { 1.0f, 1.0f, 1.0f };
		// right plane 2
		verticesCube[21].position = { 1.0f, -1.0f, -1.0f };
		verticesCube[22].position = { 1.0f, 1.0f, 1.0f };
		verticesCube[23].position = { 1.0f, 1.0f, -1.0f };
		// bottom plane 1
		verticesCube[24].position = { 1.0f, -1.0f, -1.0f };
		verticesCube[25].position = { -1.0f, -1.0f, -1.0f };
		verticesCube[26].position = { -1.0f, -1.0f, 1.0f };
		// bottom plane 2
		verticesCube[27].position = { 1.0f, -1.0f, -1.0f };
		verticesCube[28].position = { -1.0f, -1.0f, 1.0f };
		verticesCube[29].position = { 1.0f, -1.0f, 1.0f };

		// cube colors
		verticesCube[6].color = { 0.5f, 0.5f, 0.0f, 1.0f };
		verticesCube[7].color = { 0.5f, 0.5f, 0.0f, 1.0f };
		verticesCube[8].color = { 0.5f, 0.5f, 0.0f, 1.0f };
		verticesCube[9].color = { 0.5f, 0.5f, 0.0f, 1.0f };
		verticesCube[10].color = { 0.5f, 0.5f, 0.0f, 1.0f };
		verticesCube[11].color = { 0.5f, 0.5f, 0.0f, 1.0f };

		verticesCube[12].color = { 0.0f, 0.5f, 0.5f, 1.0f };
		verticesCube[13].color = { 0.0f, 0.5f, 0.5f, 1.0f };
		verticesCube[14].color = { 0.0f, 0.5f, 0.5f, 1.0f };
		verticesCube[15].color = { 0.0f, 0.5f, 0.5f, 1.0f };
		verticesCube[16].color = { 0.0f, 0.5f, 0.5f, 1.0f };
		verticesCube[17].color = { 0.0f, 0.5f, 0.5f, 1.0f };

		verticesCube[18].color = { 0.3f, 0.2f, 0.9f, 1.0f };
		verticesCube[19].color = { 0.3f, 0.2f, 0.9f, 1.0f };
		verticesCube[20].color = { 0.3f, 0.2f, 0.9f, 1.0f };
		verticesCube[21].color = { 0.3f, 0.2f, 0.9f, 1.0f };
		verticesCube[22].color = { 0.3f, 0.2f, 0.9f, 1.0f };
		verticesCube[23].color = { 0.3f, 0.2f, 0.9f, 1.0f };

		verticesCube[24].color = { 0.7f, 0.1f, 0.2f, 1.0f };
		verticesCube[25].color = { 0.7f, 0.1f, 0.2f, 1.0f };
		verticesCube[26].color = { 0.7f, 0.1f, 0.2f, 1.0f };
		verticesCube[27].color = { 0.7f, 0.1f, 0.2f, 1.0f };
		verticesCube[28].color = { 0.7f, 0.1f, 0.2f, 1.0f };
		verticesCube[29].color = { 0.7f, 0.1f, 0.2f, 1.0f };
		// --end of cube--
		*/