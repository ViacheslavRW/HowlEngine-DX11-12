#include "pch.h"
#include "DXDebug.h"

namespace HEngine
{
	DXDebug DXDebug::mInstance;

	void DXDebug::Enable()
	{
		GetInterface();
		Get()->EnableDebugLayer();
	}

	void DXDebug::GetInterface()
	{
		if (!Get())
		{
			HRESULT res = D3D12GetDebugInterface(IID_PPV_ARGS(GetAddressOf()));
			if(FAILED(res)) std::cout << "FAILED_TO_GET_DEBUG_INTERFACE" << std::endl;
		}
	}
}