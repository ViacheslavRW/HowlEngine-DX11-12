#pragma once
#include "PBRMesh.h"
#include "../Common/TextureManager.h"
#include <assimp/scene.h>

namespace HEngine
{
	class MeshLoader
	{
	public:
		MeshLoader() = default;
		~MeshLoader() = default;

		MeshLoader(const MeshLoader&) = delete;
		MeshLoader& operator=(const MeshLoader&) = delete;
	public:
		void Initialize(TextureManager* _textureManager, ID3D11Device* _device, MaterialManager* _materialManager);
		void LoadMesh(PBRMesh* pMesh, const std::string& path, const std::wstring& texturesPath);
		void Release();
	private:
		void ProcessNode(aiNode* node, const aiScene* scene, PBRMesh* pMesh, const std::wstring& texturesPath);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, PBRSubMesh* subMesh, const std::wstring& texturesPath);

		TextureFormat DetectTextureFormat(const std::wstring& filename);

		TextureManager* pTextureManager;
		ID3D11Device* pDevice;
		MaterialManager* pMaterialManager;

	private:
		const std::string defaultTextureName = "no_texture";
		const std::wstring defaultTexturePath = L"Assets/Textures/";
	};
}

