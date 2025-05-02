#include "pch.h"
#include "DX12PipelineState.h"

namespace HEngine
{
	DX12PipelineState::~DX12PipelineState()
	{
		Release();
	}

	void DX12PipelineState::Initialize(ID3D12Device* pDevice)
	{
		mRootSignature.Initialize(pDevice);
		mShaders[0].Initialize(L"shaders/VS.hlsl", HLSLShader::ShaderType::VERTEX); 
		mShaders[1].Initialize(L"shaders/PS.hlsl", HLSLShader::ShaderType::PIXEL);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = {};
		psDesc.pRootSignature = mRootSignature.Get();
		psDesc.VS.pShaderBytecode = mShaders[0].GetShaderBlob()->GetBufferPointer();
		psDesc.VS.BytecodeLength = mShaders[0].GetShaderBlob()->GetBufferSize();
		psDesc.PS.pShaderBytecode = mShaders[1].GetShaderBlob()->GetBufferPointer();
		psDesc.PS.BytecodeLength = mShaders[1].GetShaderBlob()->GetBufferSize();

		psDesc.BlendState.AlphaToCoverageEnable = false;
		psDesc.BlendState.IndependentBlendEnable = false;
		psDesc.BlendState.RenderTarget[0].BlendEnable = false;
		psDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		psDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		psDesc.SampleMask = UINT_MAX;
		psDesc.SampleDesc = { 1,0 };

		psDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // check later if needed
		psDesc.RasterizerState.FrontCounterClockwise = true;
		psDesc.RasterizerState.DepthClipEnable = true;
		psDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT, //DXGI_FORMAT_D24_UNORM_S8_UINT;
		psDesc.DepthStencilState.DepthEnable = true;
		psDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		// other elements may be added later
		D3D12_INPUT_ELEMENT_DESC elements[3] =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
			{
				"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
			{
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			}
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = 3;
		inputLayoutDesc.pInputElementDescs = elements;

		psDesc.InputLayout = inputLayoutDesc;
		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		psDesc.NumRenderTargets = 1;
		psDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		psDesc.NodeMask = 0;
		psDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT res = pDevice->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(GetAddressOf()));
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_PIPELINE_STATE" << std::endl;
	}

	void DX12PipelineState::Release()
	{
		mRootSignature.Release();
		mShaders[0].Release();
		mShaders[1].Release();

		if (Get())
		{
			Reset();
		}
	}
}
