#include "pch.h"
#include "InputManager.h"

namespace HEngine
{
	void InputManager::Initialize(Camera* camera)
	{
		pCamera = camera;
		isKeyboardAvailable = true;
		isMouseAvailable = true;
	}

	void InputManager::Update(float deltaTime)
	{
		mKeyboard.Update();
		// Switch mode
		if (mKeyboard.isKeyReleased(VK_TAB))
		{
			if (currentInputMode == InputMode::EditorMode) SetInputMode(InputMode::GameMode);
			else SetInputMode(InputMode::EditorMode);
		}
			
		if (currentInputMode == InputMode::EditorMode) mEditorInputMode.HandleInput(mKeyboard, mMouse, pCamera, deltaTime);
		else mGameInputMode.HandleInput(mKeyboard, mMouse, pCamera ,deltaTime);
	}

	void EditorInputMode::HandleInput(Keyboard& keyboard, Mouse& mouse, Camera* pCamera, float& deltaTime)
	{
		// Camera movement
		if (keyboard.isKeyDown('W')) pCamera->MoveForward(deltaTime);
		if (keyboard.isKeyDown('A')) pCamera->MoveLeft(deltaTime);
		if (keyboard.isKeyDown('S')) pCamera->MoveBackward(deltaTime);
		if (keyboard.isKeyDown('D')) pCamera->MoveRight(deltaTime);
		if (keyboard.isKeyDown('R')) pCamera->MoveUp(deltaTime);
		if (keyboard.isKeyDown('F')) pCamera->MoveDown(deltaTime);
		// Camera rotation
		if (keyboard.isKeyDown('Q')) pCamera->RotateLeft(deltaTime);
		if (keyboard.isKeyDown('E')) pCamera->RotateRight(deltaTime);
		if (keyboard.isKeyDown('Z')) pCamera->RotateUp(deltaTime);
		if (keyboard.isKeyDown('X')) pCamera->RotateDown(deltaTime);
		// Toggle
		if (keyboard.isKeyReleased('C')) pCamera->SetRotationEnabled(false);
		if (keyboard.isKeyReleased('V')) pCamera->SetRotationEnabled(true);
	}

	void GameInputMode::HandleInput(Keyboard& keyboard, Mouse& mouse, Camera* pCamera, float& deltaTime)
	{

	}
}