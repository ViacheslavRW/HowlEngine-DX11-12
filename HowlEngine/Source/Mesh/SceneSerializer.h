#pragma once
#include "MeshManager.h"
#include <fstream>
#include <iostream>
#include <string>

namespace HEngine
{
	class SceneSerializer
	{
	public:
		static bool SaveBinary(MeshManager& meshManager, const std::string& fileName);
		static bool LoadBinary(MeshManager& meshManager, const std::string& fileName);

		static bool SaveJson(MeshManager& meshManager, const std::string& fileName);
		static bool LoadJson(MeshManager& meshManager, const std::string& fileName);
	};
}


