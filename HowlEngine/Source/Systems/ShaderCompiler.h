#pragma once
#include "../Renderer/RenderTypes/GraphicsAPI.h"

namespace HEngine
{
	using namespace Microsoft::WRL;
	struct ShaderCompiler
	{
	public:
		enum class ShaderPipeline
		{
			UNIVERSAL,
			GLASS
		};

		enum class ShaderType
		{
			VERTEX,
			PIXEL,
			GEOMETRY,
		};

	public:
		ShaderCompiler() = default;

		void Compile(const LPCWSTR filename, const ShaderType shaderType, const GraphicsAPI api, ID3DBlob** bytecode);
		void CompileAll();
		void CreateAll(ComPtr<ID3D11Device>& pDevice);

		void SetShadersToPipeline(ComPtr<ID3D11DeviceContext>& pDeviceContext, ShaderPipeline pipeline) const;
		void Release();
	public:
		// blob
		ComPtr<ID3DBlob> vsBlobUniversal;
		ComPtr<ID3DBlob> psBlobUniversal;
		ComPtr<ID3DBlob> vsBlobGlass;
		ComPtr<ID3DBlob> psBlobGlass;
		// shaders
		ComPtr<ID3D11VertexShader> mVertexShaderUniversal;
		ComPtr<ID3D11PixelShader> mPixelShaderUniversal;
		ComPtr<ID3D11VertexShader> mVertexShaderGlass;
		ComPtr<ID3D11PixelShader> mPixelShaderGlass;
	private:
		std::string EnumToString(ShaderType type);
	};
}


