#pragma once
#include "Windows.h"
#include "Keyboard.h"
#include "../Common/Camera.h"
#include "../Renderer/IRenderer.h"

namespace HEngine
{
	struct Ray
	{
		XMVECTOR origin;
		XMVECTOR direction;
	};
	enum class InputMode
	{
		EditorMode,
		GameMode,
	};

	struct EditorInputMode
	{
	public:
		void HandleInput(Keyboard& keyboard, Camera* pCamera, float& deltaTime);
	};

	struct GameInputMode
	{
	public:
		void HandleInput(Keyboard& keyboard, Camera* pCamera, float& deltaTime);
	};

	struct InputManager
	{
	public:
		void Initialize(Camera* camera);
		void Update(float deltaTime);

		inline InputMode GetInputMode() const { return currentInputMode; };
		inline void SetInputMode(InputMode newMode)  { currentInputMode = newMode; };

		inline void SetCamera(Camera* camera) { pCamera = camera; };
		inline void SetRenderer(IRenderer* renderer) { pRenderer = renderer; };

		void PickMesh(const int screenX, const int screenY, const int wndWidth, const int wndHeight);

		Ray CreateMouseRay(const int screenX, const int screenY, const int wndWidth, const int wndHeight);

		Camera* pCamera = nullptr;
		IRenderer* pRenderer = nullptr;
	public:
		Keyboard mKeyboard;

	private:
		InputMode currentInputMode = InputMode::EditorMode;
		GameInputMode mGameInputMode;
		EditorInputMode mEditorInputMode;

		bool isKeyboardAvailable = false;
		bool isMouseAvailable = false;
	};
}


