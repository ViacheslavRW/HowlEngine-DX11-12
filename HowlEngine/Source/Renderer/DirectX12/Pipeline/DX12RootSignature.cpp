#include "pch.h"
#include "DX12RootSignature.h"

namespace HEngine
{
	DX12RootSignature::~DX12RootSignature()
	{
		Release();
	}

	void DX12RootSignature::Initialize(ID3D12Device* pDevice)
	{
		D3D12_ROOT_PARAMETER rootParam[2] = {};
		rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParam[0].Descriptor.ShaderRegister = 0;
		rootParam[0].Descriptor.RegisterSpace = 0;
		rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParam[1].Descriptor.ShaderRegister = 1;
		rootParam[1].Descriptor.RegisterSpace = 0;
		rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParam;
		rsDesc.Desc_1_0.NumParameters = 2;
		rsDesc.Desc_1_0.NumStaticSamplers = 0;
		rsDesc.Desc_1_0.pStaticSamplers = 0;
		rsDesc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* pSerializedRS = nullptr;
		ID3DBlob* pErrorBlob = nullptr;

		HRESULT sRes = D3D12SerializeVersionedRootSignature(&rsDesc, &pSerializedRS, &pErrorBlob);
		if (FAILED(sRes)) std::cout << "FAILED_TO_SERIALIZE_ROOT_SIGNATURE" << std::endl;
		if (pErrorBlob) std::cout << (const char*)pErrorBlob->GetBufferPointer() << std::endl;

		HRESULT cRes = pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf()));
		if (FAILED(cRes)) std::cout << "FAILED_TO_CREATE_ROOT_SIGNATURE" << std::endl;
	}

	void DX12RootSignature::Release()
	{
		if (Get())
		{
			Reset();
		}
	}
}