#pragma once
#include "Mesh.h"
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
		void LoadMesh(Mesh* pMesh, const std::string& path);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene, Mesh* pMesh);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, Mesh* pMesh);
	};
}

