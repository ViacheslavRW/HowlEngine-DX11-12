#include "pch.h"
#include "DXGIAdapter.h"

namespace HEngine
{
	DXGIAdapter::DXGIAdapter(IDXGIAdapter* pAdapter) : Microsoft::WRL::ComPtr<IDXGIAdapter>(pAdapter)
	{

	}
}