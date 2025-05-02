#include "pch.h"
#include "DXGIDebug.h"

namespace HEngine
{
	DXGIDebug DXGIDebug::mInstance;

	void DXGIDebug::Enable()
	{
		GetInterface();
	}

	void DXGIDebug::GetLiveObjects()
	{
		GetInterface();
		Get()->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}

	void DXGIDebug::GetInterface()
	{
		if (!Get())
		{
			HRESULT res = DXGIGetDebugInterface1(0, IID_PPV_ARGS(GetAddressOf()));
			if (FAILED(res)) std::cout << "FAILED_TO_GET_DXGI_DEBUG_INTERFACE" << std::endl;
		}
	}
}
