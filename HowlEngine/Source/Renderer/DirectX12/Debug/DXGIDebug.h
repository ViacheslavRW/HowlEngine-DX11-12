#pragma once

namespace HEngine
{
	class DXGIDebug : public Microsoft::WRL::ComPtr<IDXGIDebug>
	{
	public:
		DXGIDebug() = default;

		static inline DXGIDebug& Get() { return mInstance; };

		void Enable();
		void GetLiveObjects();

	private:
		void GetInterface();
	private:
		static DXGIDebug mInstance;
	};
}


