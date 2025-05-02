#include "pch.h"
#include "DXDevice.h"

namespace HEngine
{
	DXDevice::DXDevice(IDXGIAdapter* pAdapter)
	{
		Initialize(pAdapter);
	}

	void DXDevice::Initialize(IDXGIAdapter* pAdapter)
	{
		HRESULT res = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(GetAddressOf()));
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_DEVICE" << std::endl;
	}
}