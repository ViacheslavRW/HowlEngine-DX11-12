#include "pch.h"
#include "MeshLoader.h"
#include "../Renderer/RenderTypes/RenderDataTypes.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace HEngine
{
	void MeshLoader::LoadMesh(Mesh* pMesh, const std::string& path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
			return;
		};
		
		ProcessNode(scene->mRootNode, scene, pMesh);
	}

	void MeshLoader::ProcessNode(aiNode* node, const aiScene* scene, Mesh* pMesh)
	{
		for (UINT i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene, pMesh);
		}

		for (UINT i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, pMesh);
		}
	}

	void MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, Mesh* pMesh)
	{
		UINT vertexOffset = static_cast<UINT>(pMesh->vertices.size());

		for (UINT i = 0; i < mesh->mNumVertices; ++i)
		{
			TR::Vertex3T vertex;

			vertex.position =
			{
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			};

			if (mesh->HasNormals())
			{
				vertex.normal =
				{
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.textureCoord =
				{
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			}
			else vertex.textureCoord = { 0.0f, 0.0f };

			pMesh->vertices.push_back(vertex);
		}

		for (UINT i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; ++j)
			{
				pMesh->indices.push_back(face.mIndices[j] + vertexOffset);
			}
		}
	}
}