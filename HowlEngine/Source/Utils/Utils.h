#pragma once


namespace HEngine
{
	namespace Utils
	{
		inline UINT CalculateConstantBufferAlignment(UINT allocation) { return (allocation + 255) & ~255; };
	}
}


