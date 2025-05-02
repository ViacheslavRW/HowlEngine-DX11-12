#pragma once

#include "wrl/client.h"
#include "d3d12.h"
#include <dxgi1_6.h>

namespace HEngine
{
	class DXDevice : public Microsoft::WRL::ComPtr<ID3D12Device>
	{
	public:
		DXDevice() = default;
		DXDevice(IDXGIAdapter* pAdapter);

		void Initialize(IDXGIAdapter* pAdapter);
	};
}


