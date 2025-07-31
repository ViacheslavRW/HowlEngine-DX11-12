#include "pch.h"
#include "ShaderCompiler.h"

namespace HEngine
{
	void ShaderCompiler::Compile(const LPCWSTR filename, const ShaderType shaderType, const GraphicsAPI api, ID3DBlob** bytecode)
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ALL_RESOURCES_BOUND;

#if defined(DEBUG) || defined(_DEBUG)
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif 

		const char* target = "";

		switch (shaderType)
		{
			case ShaderType::VERTEX:
			{
				if( api == GraphicsAPI::DirectX11) target = "vs_5_0";
				if( api == GraphicsAPI::DirectX12) target = "vs_5_1";
				break;
			}
			case ShaderType::PIXEL:
			{
				if (api == GraphicsAPI::DirectX11) target = "ps_5_0";
				if (api == GraphicsAPI::DirectX12) target = "ps_5_1";
				break;
			}
			case ShaderType::GEOMETRY:
			{
				if (api == GraphicsAPI::DirectX11) target = "gs_5_0";
				if (api == GraphicsAPI::DirectX12) target = "gs_5_1";
				break;
			}
			default:
			{
				std::cout << "UNKNOWN_SHADER_TYPE" << std::endl;
				return;
			}
		}

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

		HRESULT res = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target, flags, 0, bytecode, errorBlob.GetAddressOf());
		if (FAILED(res))
		{
			std::cout << "FAILED_TO_COMPILE_" << EnumToString(shaderType) << "_SHADER" << std::endl;
		}
		if (errorBlob)
		{
			std::cout << "SHADER_ERROR_MESSAGE: " << (const char*)errorBlob->GetBufferPointer() << std::endl;
			return;
		}
	}

	void ShaderCompiler::CompileAll()
	{
		Compile(L"Shaders/PBRVertexShader.hlsl", ShaderCompiler::ShaderType::VERTEX, GraphicsAPI::DirectX11, &vsBlobPBR);
		Compile(L"Shaders/PBRPixelShader.hlsl", ShaderCompiler::ShaderType::PIXEL, GraphicsAPI::DirectX11, &psBlobPBR);
		Compile(L"Shaders/PBRVertexShader.hlsl", ShaderCompiler::ShaderType::VERTEX, GraphicsAPI::DirectX11, &vsBlobPBRTransparent);
		Compile(L"Shaders/PBRPixelShader.hlsl", ShaderCompiler::ShaderType::PIXEL, GraphicsAPI::DirectX11, &psBlobPBRTransparent);

		Compile(L"Shaders/VertexShader.hlsl", ShaderCompiler::ShaderType::VERTEX, GraphicsAPI::DirectX11, &vsBlobUniversal);
		Compile(L"Shaders/PixelShader.hlsl", ShaderCompiler::ShaderType::PIXEL, GraphicsAPI::DirectX11, &psBlobUniversal);
		Compile(L"Shaders/GlassVertexShader.hlsl", ShaderCompiler::ShaderType::VERTEX, GraphicsAPI::DirectX11, &vsBlobGlass);
		Compile(L"Shaders/GlassPixelShader.hlsl", ShaderCompiler::ShaderType::PIXEL, GraphicsAPI::DirectX11, &psBlobGlass);
	}

	void ShaderCompiler::CreateAll(ComPtr<ID3D11Device>& pDevice)
	{
		HRESULT res;
		res = pDevice->CreateVertexShader(vsBlobPBR->GetBufferPointer(), vsBlobPBR->GetBufferSize(), nullptr, &mPBRVertexShader);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_VERTEX_SHADER_PBR" << std::endl;
		res = pDevice->CreatePixelShader(psBlobPBR->GetBufferPointer(), psBlobPBR->GetBufferSize(), nullptr, &mPBRPixelShader);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_PIXEL_SHADER_PBR" << std::endl;
		res = pDevice->CreateVertexShader(vsBlobPBR->GetBufferPointer(), vsBlobPBRTransparent->GetBufferSize(), nullptr, &mPBRTransparentVertexShader);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_VERTEX_SHADER_PBR_TRANSPARENT" << std::endl;
		res = pDevice->CreatePixelShader(psBlobPBR->GetBufferPointer(), psBlobPBRTransparent->GetBufferSize(), nullptr, &mPBRTransparentPixelShader);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_PIXEL_SHADER_PBR_TRANSPARENT" << std::endl;

		res = pDevice->CreateVertexShader(vsBlobUniversal->GetBufferPointer(), vsBlobUniversal->GetBufferSize(), nullptr, &mVertexShaderUniversal);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_VERTEX_SHADER_UNIVERSAL" << std::endl;
		res = pDevice->CreatePixelShader(psBlobUniversal->GetBufferPointer(), psBlobUniversal->GetBufferSize(), nullptr, &mPixelShaderUniversal);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_PIXEL_SHADER_UNIVERSAL" << std::endl;
		res = pDevice->CreateVertexShader(vsBlobGlass->GetBufferPointer(), vsBlobGlass->GetBufferSize(), nullptr, &mVertexShaderGlass);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_VERTEX_SHADER_GLASS" << std::endl;
		res = pDevice->CreatePixelShader(psBlobGlass->GetBufferPointer(), psBlobGlass->GetBufferSize(), nullptr, &mPixelShaderGlass);
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_PIXEL_SHADER_GLASS" << std::endl;
	}

	void ShaderCompiler::SetShadersToPipeline(ComPtr<ID3D11DeviceContext>& pDeviceContext, ShaderPipeline pipeline) const
	{
		switch (pipeline)
		{
			case ShaderPipeline::PBR:
			{
				pDeviceContext->VSSetShader(mPBRVertexShader.Get(), nullptr, 0);
				pDeviceContext->PSSetShader(mPBRPixelShader.Get(), nullptr, 0);
			} break;
			case ShaderPipeline::PBR_TRANSPARENT:
			{
				pDeviceContext->VSSetShader(mPBRTransparentVertexShader.Get(), nullptr, 0);
				pDeviceContext->PSSetShader(mPBRTransparentPixelShader.Get(), nullptr, 0);
			} break;
			case ShaderPipeline::UNIVERSAL:
			{
				pDeviceContext->VSSetShader(mVertexShaderUniversal.Get(), nullptr, 0);
				pDeviceContext->PSSetShader(mPixelShaderUniversal.Get(), nullptr, 0);
			} break;
			case ShaderPipeline::GLASS:
			{
				pDeviceContext->VSSetShader(mVertexShaderGlass.Get(), nullptr, 0);
				pDeviceContext->PSSetShader(mPixelShaderGlass.Get(), nullptr, 0);
			} break;
		}
	}

	void ShaderCompiler::Release()
	{
		vsBlobPBR.Reset();
		psBlobPBR.Reset();
		vsBlobPBRTransparent.Reset();
		psBlobPBRTransparent.Reset();

		vsBlobUniversal.Reset();
		psBlobUniversal.Reset();
		vsBlobGlass.Reset();
		psBlobGlass.Reset();

		mPBRVertexShader.Reset();
		mPBRPixelShader.Reset();
		mPBRTransparentVertexShader.Reset();
		mPixelShaderUniversal.Reset();

		mVertexShaderUniversal.Reset();
		mPixelShaderUniversal.Reset();
		mVertexShaderGlass.Reset();
		mPixelShaderGlass.Reset();
	}

	std::string ShaderCompiler::EnumToString(ShaderType type)
	{
		switch (type) {
		case ShaderType::VERTEX: return "VERTEX";
		case ShaderType::PIXEL: return "PIXEL";
		case ShaderType::GEOMETRY: return "GEOMETRY";
		default: return "Unknown";
		}
	}
}