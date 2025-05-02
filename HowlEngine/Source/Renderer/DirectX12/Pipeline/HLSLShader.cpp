#include "pch.h"
#include "HLSLShader.h"

namespace HEngine
{
	HLSLShader::~HLSLShader()
	{
		Release();
	}

	void HLSLShader::Initialize(const LPCWSTR filename, const ShaderType shaderType)
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
				target = "vs_5_1";
				break;
			}
			case ShaderType::PIXEL:
			{
				target = "ps_5_1";
				break;
			}
			default:
			{
				std::cout << "UNKNOWN_SHADER_TYPE" << std::endl;
				return;
			}
		}

		ID3DBlob* errorBlob = nullptr;

		HRESULT res = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target, flags, 0, &mShaderBlob, &errorBlob);

		if (FAILED(res))
		{
			std::cout << "SHADER_COMPILATION_ERROR" << std::endl;
			const char* shaderTypeRev[]{"VERTEX", "PIXEL"};
			std::cout << "FAILED_TO_COMPILE_" << shaderTypeRev[shaderType] << "_SHADER" << std::endl;
		}
		if (errorBlob)
		{
			std::cout << "SHADER_ERROR_MESSAGE: " << (const char*)errorBlob->GetBufferPointer() << std::endl;
			return;
		}
	}

	void HLSLShader::Release()
	{
		if (mShaderBlob && mShaderBlob->GetBufferPointer())
		{
			mShaderBlob->Release();
			mShaderBlob = nullptr;
		}
	}
}
