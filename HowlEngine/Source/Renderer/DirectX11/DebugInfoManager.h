#pragma once

namespace HEngine
{
	class DebugInfoManager
	{
	public:
		DebugInfoManager();
		DebugInfoManager(const DebugInfoManager&) = delete;
		DebugInfoManager* operator=(const DebugInfoManager&) = delete;
		~DebugInfoManager();

		void Set() noexcept;
		std::vector<std::string> GetDebugMessages() const;

	private:
		UINT64 index = 0;
		struct IDXGIInfoQueue* mDXGIInfoQueue = nullptr;
		HMODULE mDXGIDebugLib = nullptr;
	};
}

