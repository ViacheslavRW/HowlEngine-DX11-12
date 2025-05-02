#pragma once
#include <wrl.h>

#include "DX12RootSignature.h"
#include "HLSLShader.h"

namespace HEngine
{
	class DX12PipelineState : public Microsoft::WRL::ComPtr<ID3D12PipelineState>
	{
	public:
		DX12PipelineState() = default;
		~DX12PipelineState();

		void Initialize(ID3D12Device* pDevice);

		void Release();

	public:
		inline ID3D12RootSignature* GetRootSugnature() { return mRootSignature.Get(); };

	private:
		DX12RootSignature mRootSignature;
		HLSLShader mShaders[2];
	};
}


