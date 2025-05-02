#include "pch.h"
#include "DebugInfoManager.h"

namespace HEngine
{
	DebugInfoManager::DebugInfoManager()
	{
		typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

		mDXGIDebugLib = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if(mDXGIDebugLib == nullptr) std::cout << "FAILED_TO_LOAD_DXGI_LIB" << std::endl;

		const auto dxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
			GetProcAddress(mDXGIDebugLib, "DXGIGetDebugInterface")
			);
		if(dxgiGetDebugInterface == nullptr) std::cout << "FAILED_TO_GET_DXGI_DEBUG_FUNCTION" << std::endl;

		HRESULT hr = dxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&mDXGIInfoQueue));
		if (FAILED(hr)) std::cout << "FAILED_TO_GET_DXGI_DEBUG" << std::endl;
	}

	DebugInfoManager::~DebugInfoManager()
	{
		if (mDXGIInfoQueue) mDXGIInfoQueue->Release();
		if (mDXGIDebugLib) FreeLibrary(mDXGIDebugLib);
	}

	void DebugInfoManager::Set() noexcept
	{
		index = mDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	}

	std::vector<std::string> DebugInfoManager::GetDebugMessages() const
	{
		std::vector<std::string> messages;
		const auto end = mDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

		for (auto i = index; i < end; i++)
		{
			SIZE_T messageLength;
			mDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);

			auto bytes = std::make_unique<byte[]>(messageLength);
			auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

			mDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
			messages.emplace_back(pMessage->pDescription);
		}

		return messages;
	}
}