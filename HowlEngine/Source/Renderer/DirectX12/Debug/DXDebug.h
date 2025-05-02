#pragma once

namespace HEngine
{
	class DXDebug : public Microsoft::WRL::ComPtr<ID3D12Debug>
	{
	public:
		DXDebug() = default;

		static inline DXDebug& Get() { return mInstance; };

		void Enable();

	private:
		void GetInterface();

	private:
		static DXDebug mInstance;
	};
}


