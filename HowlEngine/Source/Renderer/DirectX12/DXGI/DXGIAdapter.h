#pragma once

namespace HEngine
{
	class DXGIAdapter : public Microsoft::WRL::ComPtr<IDXGIAdapter>
	{
	public:
		DXGIAdapter() = default;
		DXGIAdapter(IDXGIAdapter* pAdapter);
	};

}

