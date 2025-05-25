#pragma once
#include "Windows.h"
#include <array>
#include <vector>

namespace HEngine
{
	struct Keyboard
	{
	public:
		void Update();

		bool isKeyDown(int key) const;
		bool isKeyPressed(int key) const;
		bool isKeyReleased(int key) const;

	private:
		static constexpr int numTrackedKeys = 13;
		std::array<bool, numTrackedKeys> currentKeys{};
		std::array<bool, numTrackedKeys> previousKeys{};
		std::array<int, numTrackedKeys> trackedKeys
		{
			'W', 'A', 'S', 'D', 'R', 'F',
			'Q', 'E', 'Z', 'X', 'C', 'V',
			VK_TAB
		};
	};
}


