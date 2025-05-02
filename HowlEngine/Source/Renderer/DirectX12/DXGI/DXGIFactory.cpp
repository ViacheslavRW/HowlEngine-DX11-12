#include "pch.h"
#include "DXGIFactory.h"

namespace HEngine
{
	using namespace Microsoft::WRL;

	DXGIFactory::DXGIFactory()
	{
		UINT flags = {};
#if defined(DEBUG) || defined(_DEBUG)
		flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT res = CreateDXGIFactory2(flags, IID_PPV_ARGS(&ptr_));
		if (FAILED(res)) std::cout << "FAILED_TO_CREATE_FACTORY" << std::endl;
	}

	DXGIAdapter DXGIFactory::GetAdapter()
	{
		ComPtr<IDXGIFactory6> factory = nullptr;
		ComPtr<IDXGIAdapter> adapter;

		if ((Get()->QueryInterface(IID_PPV_ARGS(&factory))) == S_OK)
		{
			HRESULT res = factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
			if(res == DXGI_ERROR_NOT_FOUND) std::cout << "ADAPTER_NOT_FOUND" << std::endl;
		}

		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		std::wcout << L"Selected GPU: " << desc.Description << std::endl;
		LogAdapterOutputs(adapter.Get());

		return DXGIAdapter(adapter.Get());
	}

	void DXGIFactory::LogAdapterOutputs(IDXGIAdapter* pAdapter)
	{
		UINT outputIndex = 0;
		ComPtr<IDXGIOutput> output;

		while (pAdapter->EnumOutputs(outputIndex, &output) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc = {};
			output->GetDesc(&desc);

			std::wcout << L"Device Name: " << desc.DeviceName << std::endl;
			//std::wcout << L"Monitor: " << desc.Monitor << std::endl;

			outputIndex++;
			output.Reset();
		}
	}
}