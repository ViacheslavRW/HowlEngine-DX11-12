#include "pch.h"
#include "Keyboard.h"

namespace HEngine
{
	void Keyboard::Update()
	{
		for (int i = 0; i < numTrackedKeys; ++i)
		{
			int key = trackedKeys[i];
			previousKeys[i] = currentKeys[i];
			currentKeys[i] = (GetAsyncKeyState(key) & 0x8000) != 0;
		}
	}

	bool Keyboard::isKeyDown(int key) const
	{
		for (int i = 0; i < numTrackedKeys; ++i)
			if (trackedKeys[i] == key) return currentKeys[i];
				
		return false;
	}

	bool Keyboard::isKeyPressed(int key) const
	{
		for (int i = 0; i < numTrackedKeys; ++i)
			if (trackedKeys[i] == key)
				return currentKeys[i] && !previousKeys[i];
		return false;
	}

	bool Keyboard::isKeyReleased(int key) const
	{
		for (int i = 0; i < numTrackedKeys; ++i)
			if (trackedKeys[i] == key)
				return !currentKeys[i] && previousKeys[i];
		return false;
	}
}