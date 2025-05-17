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
		static constexpr int numTrackedKeys = 7;
		std::array<bool, numTrackedKeys> currentKeys{};
		std::array<bool, numTrackedKeys> previousKeys{};
		std::array<int, numTrackedKeys> trackedKeys
		{
			'W', 'A', 'S', 'D', 'R', 'F',
			VK_TAB
		};
	};
}


