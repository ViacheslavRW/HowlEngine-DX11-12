#pragma once

#include "DXGIAdapter.h"

namespace HEngine
{
	class DXGIFactory : public Microsoft::WRL::ComPtr<IDXGIFactory2>
	{
	public:
		DXGIFactory();

		DXGIAdapter GetAdapter();

		void LogAdapterOutputs(IDXGIAdapter* pAdapter);
	};
}


