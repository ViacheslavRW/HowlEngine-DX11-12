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