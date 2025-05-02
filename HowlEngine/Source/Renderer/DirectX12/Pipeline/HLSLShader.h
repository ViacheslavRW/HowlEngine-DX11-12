#pragma once

namespace HEngine
{
	class HLSLShader
	{
	public:
		enum ShaderType
		{
			VERTEX,
			PIXEL
		};

	public:
		HLSLShader() = default;
		~HLSLShader();

		void Initialize(const LPCWSTR filename, const ShaderType shaderType);
		void Release();

	public:
		inline ID3DBlob* GetShaderBlob() { return mShaderBlob; };

	private:
		ID3DBlob* mShaderBlob = nullptr;
	};
}


