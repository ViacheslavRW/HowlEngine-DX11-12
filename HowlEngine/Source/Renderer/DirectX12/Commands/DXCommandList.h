#pragma once
#include <wrl.h>

namespace HEngine
{
	class DXCommandList : public Microsoft::WRL::ComPtr<ID3D12CommandList>
	{
	public:
		DXCommandList() = default;
		~DXCommandList();

		void Initialize(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE cmdType, ID3D12PipelineState* initPipelineState = nullptr);
		void ResetCommandList();
		void Release();

	public:
		inline ID3D12GraphicsCommandList* GetGFXCommandList() { return mGFXCommandList; };

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		ID3D12GraphicsCommandList* mGFXCommandList = nullptr;
	};
}

