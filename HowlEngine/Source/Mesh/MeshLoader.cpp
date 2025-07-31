#include "pch.h"
#include "MeshLoader.h"
#include "../Renderer/RenderTypes/RenderDataTypes.h"
#include "./MaterialManager.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <filesystem> 

namespace HEngine
{
	void MeshLoader::Initialize(TextureManager* _textureManager, ID3D11Device* _device, MaterialManager* _materialManager)
	{
		pTextureManager = _textureManager;
		pDevice = _device;
		pMaterialManager = _materialManager;
	}

	void MeshLoader::LoadMesh(PBRMesh* pMesh, const std::string& path, const std::wstring& texturesPath)
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

		ProcessNode(scene->mRootNode, scene, pMesh, texturesPath);
	}

	void MeshLoader::ProcessNode(aiNode* node, const aiScene* scene, PBRMesh* pMesh, const std::wstring& texturesPath)
	{
		for (UINT i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			PBRSubMesh subMesh;
			ProcessMesh(mesh, scene, &subMesh, texturesPath);
			pMesh->subMeshes.push_back(std::move(subMesh));
		}

		for (UINT i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, pMesh, texturesPath);
		}
	}

	void MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, PBRSubMesh* subMesh, const std::wstring& texturesPath)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		PBRMaterial mat;
		aiString name;
		if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
			mat.name = name.C_Str();
		else
			mat.name = "Unnamed";

		// set default textures
		mat.albedoSRV = pTextureManager->LoadAndGetSRV(defaultAlbedoName, defaultTexturePath, pDevice, TextureFormat::PNG);

		// Helper to load a texture
		auto loadTexture = [&](aiTextureType type, std::string& pathField, ID3D11ShaderResourceView*& srvField)
			{
				aiString path;
				if (material->GetTexture(type, 0, &path) == AI_SUCCESS)
				{
					std::string texName = std::filesystem::path(path.C_Str()).filename().string();
					std::wstring fullPath = texturesPath + std::wstring(texName.begin(), texName.end());

					TextureFormat format = DetectTextureFormat(fullPath);
					srvField = pTextureManager->LoadAndGetSRV(texName, fullPath, pDevice, format);
					pathField = texName;
				}
			};

		// Load all maps
		loadTexture(aiTextureType_DIFFUSE, mat.albedoPath, mat.albedoSRV);
		loadTexture(aiTextureType_NORMALS, mat.normalPath, mat.normalSRV);
		loadTexture(aiTextureType_METALNESS, mat.ormPath, mat.ormSRV); // assimp considers metalness or roughness as ORM

		subMesh->material = std::move(mat);

		for (UINT i = 0; i < mesh->mNumVertices; ++i)
		{
			TR::PBRVertex vertex;

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

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tangent =
				{
					mesh->mTangents[i].x,
					mesh->mTangents[i].y,
					mesh->mTangents[i].z
				};

				vertex.bitangent =
				{
					mesh->mBitangents[i].x,
					mesh->mBitangents[i].y,
					mesh->mBitangents[i].z
				};
			}
			else
			{
				vertex.tangent = XMFLOAT3(1, 0, 0);
				vertex.bitangent = XMFLOAT3(0, 1, 0);
			}

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

	void MeshLoader::Release()
	{
		pTextureManager = nullptr;
		pDevice = nullptr;
		pMaterialManager = nullptr;
	};
}