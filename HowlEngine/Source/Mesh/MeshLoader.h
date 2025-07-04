#pragma once
#include "Mesh.h"
#include "../Renderer/DirectX11/TextureManager.h"
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
		void LoadMesh(Mesh* pMesh, const std::string& path, const std::wstring& texturesPath, TextureManager* pTextureManager, ID3D11Device* pDevice);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene, Mesh* pMesh, const std::unordered_map<UINT, std::string>& materialIndexToTextureKey);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, SubMesh* subMesh, const std::unordered_map<UINT, std::string>& materialIndexToTextureKey);

		TextureFormat DetectTextureFormat(const std::wstring& filename);
	};
}

