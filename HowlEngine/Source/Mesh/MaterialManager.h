#pragma once

namespace HEngine
{
	using namespace DirectX;
	struct PBRMaterial
	{
		std::string name;

		std::string albedoPath;
		std::string normalPath;
		std::string ormPath;
		std::string emissivePath;

		bool useAlpha = false;

		ID3D11ShaderResourceView* albedoSRV = nullptr;
		ID3D11ShaderResourceView* normalSRV = nullptr;
		ID3D11ShaderResourceView* ormSRV = nullptr;
		ID3D11ShaderResourceView* emissiveSRV = nullptr;
	};

	class MaterialManager
	{
	public:
		MaterialManager() = default;
		~MaterialManager() = default;

		MaterialManager(const MaterialManager&) = delete;
		MaterialManager& operator=(const MaterialManager&) = delete;

		const XMFLOAT3 baseColor = { 1.0f, 1.0f, 1.0f };
		const float metallicFactor = 0.0f;
		const float roughnessFactor = 0.5f;
		const float aoFactor = 1.0f;
	};
}


