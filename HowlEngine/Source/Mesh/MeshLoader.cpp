#include "pch.h"
#include "MeshLoader.h"
#include "../Renderer/RenderTypes/RenderDataTypes.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <filesystem> 

namespace HEngine
{
	void MeshLoader::LoadMesh(Mesh* pMesh, const std::string& path, const std::wstring& texturesPath, TextureManager* pTextureManager, ID3D11Device* pDevice)
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

		// Load textures
		std::unordered_map<UINT, std::string> materialIndexToTextureKey;
		for (UINT i = 0; i < scene->mNumMaterials; ++i)
		{
			aiMaterial* material = scene->mMaterials[i];
			aiString texturePath;

			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
			{
				std::filesystem::path fullPath = texturePath.C_Str();
				std::string textureKey = fullPath.filename().string();

				std::wstring textureFullPath = L"Assets/Textures" + texturesPath + std::wstring(textureKey.begin(), textureKey.end());

				TextureFormat textureFormat = DetectTextureFormat(textureFullPath);
				pTextureManager->LoadTexture(textureKey, textureFullPath, pDevice, textureFormat);

				materialIndexToTextureKey[i] = textureKey;
			}
		}

		ProcessNode(scene->mRootNode, scene, pMesh, materialIndexToTextureKey);
	}

	void MeshLoader::ProcessNode(aiNode* node, const aiScene* scene, Mesh* pMesh,
		const std::unordered_map<UINT, std::string>& materialIndexToTextureKey)
	{
		for (UINT i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			SubMesh subMesh;
			ProcessMesh(mesh, scene, &subMesh, materialIndexToTextureKey);
			pMesh->subMeshes.push_back(std::move(subMesh));
		}

		for (UINT i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, pMesh, materialIndexToTextureKey);
		}
	}

	void MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, SubMesh* subMesh, 
		const std::unordered_map<UINT, std::string>& materialIndexToTextureKey)
	{
		UINT materialIndex = mesh->mMaterialIndex;
		if (materialIndexToTextureKey.find(materialIndex) != materialIndexToTextureKey.end())
			subMesh->texture = materialIndexToTextureKey.at(materialIndex);

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

			subMesh->vertices.push_back(vertex);
		}

		for (UINT i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; ++j)
			{
				subMesh->indices.push_back(face.mIndices[j]);
			}
		}
	}

	TextureFormat MeshLoader::DetectTextureFormat(const std::wstring& filename)
	{
		std::wstring ext = std::filesystem::path(filename).extension().wstring();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

		if (ext == L".tga") return TextureFormat::TGA;
		if (ext == L".png") return TextureFormat::PNG;
		if (ext == L".jpg" || ext == L".jpeg") return TextureFormat::JPG;

		return TextureFormat::PNG;
	}
}