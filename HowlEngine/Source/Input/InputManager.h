#pragma once
#include "Windows.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "../Camera/Camera.h"

namespace HEngine
{
	enum class InputMode
	{
		EditorMode,
		GameMode,
	};

	struct EditorInputMode
	{
	public:
		void HandleInput(Keyboard& keyboard, Mouse& mouse, Camera* pCamera, float& deltaTime);
	};

	struct GameInputMode
	{
	public:
		void HandleInput(Keyboard& keyboard, Mouse& mouse, Camera* pCamera, float& deltaTime);
	};

	struct InputManager
	{
	public:
		void Initialize(Camera* camera);
		void Update(float deltaTime);

		inline InputMode GetInputMode() const { return currentInputMode; };
		inline void SetInputMode(InputMode newMode)  { currentInputMode = newMode; };

		Camera* pCamera = nullptr;
	private:
		Keyboard mKeyboard;
		Mouse mMouse;
		InputMode currentInputMode = InputMode::EditorMode;
		GameInputMode mGameInputMode;
		EditorInputMode mEditorInputMode;

		bool isKeyboardAvailable = false;
		bool isMouseAvailable = false;
	};
}


