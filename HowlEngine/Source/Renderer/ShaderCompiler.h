#pragma once
#include "./RenderTypes/GraphicsAPI.h"

namespace HEngine
{
	struct ShaderCompiler
	{
	public:
		enum class ShaderType
		{
			VERTEX,
			PIXEL,
			GEOMETRY,
		};

	public:
		ShaderCompiler() = default;

		void Compile(const LPCWSTR filename, const ShaderType shaderType, const GraphicsAPI api, ID3DBlob** bytecode);

	private:
		std::string EnumToString(ShaderType type);
	};
}


