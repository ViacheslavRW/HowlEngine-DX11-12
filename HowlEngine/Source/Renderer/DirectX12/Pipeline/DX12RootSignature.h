#pragma once

namespace HEngine
{
	class DX12RootSignature : public Microsoft::WRL::ComPtr<ID3D12RootSignature>
	{
	public:
		DX12RootSignature() = default;
		~DX12RootSignature();

		void Initialize(ID3D12Device* pDevice);
		void Release();
	};
}


