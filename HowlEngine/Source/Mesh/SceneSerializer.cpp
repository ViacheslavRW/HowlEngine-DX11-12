#include "pch.h"
#include "SceneSerializer.h"
#include <iomanip>

namespace HEngine
{
	bool SceneSerializer::SaveBinary(MeshManager& meshManager, const std::string& fileName)
	{
		std::ofstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			std::cout << "FAILED_TO_SAVE_BINARY: " << fileName << std::endl;
			return false;
		}

		const char magic[4] = { 'H', 'S', 'A', 'V' };
		uint32_t version = 1;
		file.write(magic, 4);
		file.write(reinterpret_cast<const char*>(&version), sizeof(version));

		uint32_t count = (uint32_t)meshManager.meshes.size();
		file.write(reinterpret_cast<const char*>(&count), sizeof(count));

		for (const auto& mesh : meshManager.meshes)
		{
			uint32_t len = (uint32_t)mesh->modelPath.size();
			file.write(reinterpret_cast<const char*>(&len), sizeof(len));
			file.write(mesh->modelPath.data(), len);

			len = (uint32_t)mesh->texturesPath.size();
			file.write(reinterpret_cast<const char*>(&len), sizeof(len));
			file.write(reinterpret_cast<const char*>(mesh->texturesPath.data()), len * sizeof(wchar_t));

			auto& t = mesh->transform;
			file.write(reinterpret_cast<const char*>(&t.GetPosition()), sizeof(DirectX::XMFLOAT3));
			file.write(reinterpret_cast<const char*>(&t.GetRotation()), sizeof(DirectX::XMFLOAT3));
			file.write(reinterpret_cast<const char*>(&t.GetScale()), sizeof(DirectX::XMFLOAT3));
		}

		std::cout << "Binary saved: " << fileName << std::endl;
		return true;
	}

	bool SceneSerializer::LoadBinary(MeshManager& meshManager, const std::string& fileName, bool generateMips)
	{
		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			std::cout << "ERROR_OPENING_BINARY_FILE: " << fileName << std::endl;
			return false;
		}

		char magic[4];
		uint32_t version;
		file.read(magic, 4);
		file.read(reinterpret_cast<char*>(&version), sizeof(version));

		uint32_t count;
		file.read(reinterpret_cast<char*>(&count), sizeof(count));

		meshManager.meshes.clear();

		for (uint32_t i = 0; i < count; ++i)
		{
			auto mesh = std::make_unique<PBRMesh>();

			uint32_t len;
			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			mesh->modelPath.resize(len);
			file.read(&mesh->modelPath[0], len);

			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			mesh->texturesPath.resize(len);
			file.read(reinterpret_cast<char*>(&mesh->texturesPath[0]), len * sizeof(wchar_t));

			DirectX::XMFLOAT3 pos, rot, scale;
			file.read(reinterpret_cast<char*>(&pos), sizeof(pos));
			file.read(reinterpret_cast<char*>(&rot), sizeof(rot));
			file.read(reinterpret_cast<char*>(&scale), sizeof(scale));

			mesh->transform.SetPosition(pos);
			mesh->transform.SetRotation(rot);
			mesh->transform.SetScale(scale);

			if (meshManager.mMeshLoader)
			{
				meshManager.mMeshLoader->LoadMesh(mesh.get(), mesh->modelPath, mesh->texturesPath, generateMips);
			}

			meshManager.meshes.push_back(std::move(mesh));
		}

		meshManager.CreateAllBuffers();
		std::cout << "Binary loaded: " << count << " meshes" << std::endl;

		return true;
	}

	bool SceneSerializer::SaveJson(MeshManager& meshManager, const std::string& fileName)
	{
		std::ofstream file(fileName);
		if (!file.is_open())
		{
			std::cout << "FAILED_TO_SAVE_JSON: " << fileName << std::endl;
			return false;
		}

		file << "{\n";
		file << "  \"version\": 1,\n";
		file << "  \"meshes\": [\n";

		for (size_t i = 0; i < meshManager.meshes.size(); ++i)
		{
			const auto& mesh = meshManager.meshes[i];
			auto& t = mesh->transform;

			file << "    {\n";
			file << "      \"modelPath\": \"" << mesh->modelPath << "\",\n";

			std::string texPath(mesh->texturesPath.begin(), mesh->texturesPath.end());
			file << "      \"texturesPath\": \"" << texPath << "\",\n";

			file << "      \"position\": [" << t.GetPosition().x << ", " << t.GetPosition().y << ", " << t.GetPosition().z << "],\n";
			file << "      \"rotation\": [" << t.GetRotation().x << ", " << t.GetRotation().y << ", " << t.GetRotation().z << "],\n";
			file << "      \"scale\": [" << t.GetScale().x << ", " << t.GetScale().y << ", " << t.GetScale().z << "]\n";

			file << "    }";
			if (i < meshManager.meshes.size() - 1) file << ",";
			file << "\n";
		}

		file << "  ]\n";
		file << "}\n";

		std::cout << "JSON saved: " << fileName << std::endl;

		return true;
	}

	bool SceneSerializer::LoadJson(MeshManager& meshManager, const std::string& fileName, bool generateMips)
	{
		std::ifstream file(fileName);
		if (!file.is_open())
		{
			std::cout << "ERROR_OPENING_JSON_FILE: " << fileName << std::endl;
			return false;
		}

		meshManager.meshes.clear();

		std::string line;
		int meshCount = 0;

		while (std::getline(file, line))
		{
			if (line.find("modelPath") != std::string::npos)
			{
				auto mesh = std::make_unique<PBRMesh>();

				// === 1. modelPath ===
				size_t start = line.find('"', line.find(":")) + 1;
				size_t end = line.find('"', start);
				if (start < end)
					mesh->modelPath = line.substr(start, end - start);

				// === 2. texturesPath ===
				std::getline(file, line);
				if (line.find("texturesPath") != std::string::npos)
				{
					start = line.find('"', line.find(":")) + 1;
					end = line.find('"', start);
					if (start < end)
					{
						std::string tex = line.substr(start, end - start);
						mesh->texturesPath = std::wstring(tex.begin(), tex.end());
					}
				}

				// === 3. position ===
				std::getline(file, line);
				if (line.find("position") != std::string::npos)
				{
					// Example: "position": [-1.5, -0.5, 0]
					DirectX::XMFLOAT3 pos = { 0, 0, 0 };
					size_t first = line.find('[') + 1;
					size_t comma1 = line.find(',', first);
					size_t comma2 = line.find(',', comma1 + 1);

					if (first != std::string::npos)
					{
						pos.x = std::stof(line.substr(first, comma1 - first));
						pos.y = std::stof(line.substr(comma1 + 1, comma2 - comma1 - 1));
						pos.z = std::stof(line.substr(comma2 + 1, line.find(']') - comma2 - 1));
					}
					mesh->transform.SetPosition(pos);
				}

				// === 4. rotation ===
				std::getline(file, line);
				if (line.find("rotation") != std::string::npos)
				{
					DirectX::XMFLOAT3 rot = { 0, 0, 0 };
					size_t first = line.find('[') + 1;
					size_t comma1 = line.find(',', first);
					size_t comma2 = line.find(',', comma1 + 1);

					if (first != std::string::npos)
					{
						rot.x = std::stof(line.substr(first, comma1 - first));
						rot.y = std::stof(line.substr(comma1 + 1, comma2 - comma1 - 1));
						rot.z = std::stof(line.substr(comma2 + 1, line.find(']') - comma2 - 1));
					}
					mesh->transform.SetRotation(rot);
				}

				// === 5. scale ===
				std::getline(file, line);
				if (line.find("scale") != std::string::npos)
				{
					DirectX::XMFLOAT3 scale = { 1, 1, 1 };
					size_t first = line.find('[') + 1;
					size_t comma1 = line.find(',', first);
					size_t comma2 = line.find(',', comma1 + 1);

					if (first != std::string::npos)
					{
						scale.x = std::stof(line.substr(first, comma1 - first));
						scale.y = std::stof(line.substr(comma1 + 1, comma2 - comma1 - 1));
						scale.z = std::stof(line.substr(comma2 + 1, line.find(']') - comma2 - 1));
					}
					mesh->transform.SetScale(scale);
				}

				// Load the model
				if (meshManager.mMeshLoader && !mesh->modelPath.empty())
				{
					meshManager.mMeshLoader->LoadMesh(mesh.get(), mesh->modelPath, mesh->texturesPath, generateMips);
				}

				meshManager.meshes.push_back(std::move(mesh));
				meshCount++;
			}
		}

		meshManager.CreateAllBuffers();
		std::cout << "JSON loaded " << meshCount << " meshes" << std::endl;
		return true;
	}
}