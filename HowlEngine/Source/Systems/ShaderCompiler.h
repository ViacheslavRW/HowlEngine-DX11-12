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
			PBR,
			PBR_TRANSPARENT,
			SKYBOX,
			SKYBOX_HDRI,
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
		ComPtr<ID3DBlob> vsBlobPBR;
		ComPtr<ID3DBlob> psBlobPBR;
		ComPtr<ID3DBlob> vsBlobPBRTransparent;
		ComPtr<ID3DBlob> psBlobPBRTransparent;

		ComPtr<ID3DBlob> vsBlobSkybox;
		ComPtr<ID3DBlob> psBlobSkybox;
		ComPtr<ID3DBlob> vsBlobSkyboxHDRI;
		ComPtr<ID3DBlob> psBlobSkyboxHDRI;

		// shaders
		ComPtr<ID3D11VertexShader> mPBRVertexShader;
		ComPtr<ID3D11PixelShader> mPBRPixelShader;
		ComPtr<ID3D11VertexShader> mPBRTransparentVertexShader;
		ComPtr<ID3D11PixelShader> mPBRTransparentPixelShader;

		ComPtr<ID3D11VertexShader> mSkyboxVertexShader;
		ComPtr<ID3D11PixelShader> mSkyboxPixelShader;
		ComPtr<ID3D11VertexShader> mSkyboxHDRIVertexShader;
		ComPtr<ID3D11PixelShader> mSkyboxHDRIPixelShader;

	private:
		std::string EnumToString(ShaderType type);
	};
}


