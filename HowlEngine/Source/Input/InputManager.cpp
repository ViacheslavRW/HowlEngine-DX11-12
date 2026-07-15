#include "pch.h"
#include "InputManager.h"
#include "../Mesh/PBRMesh.h"
#include "../Renderer/DX11Renderer.h"

namespace HEngine
{
	void InputManager::Initialize(Camera* camera)
	{
		SetCamera(camera);
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
			
		if (currentInputMode == InputMode::EditorMode) mEditorInputMode.HandleInput(mKeyboard, pCamera, deltaTime);
		else mGameInputMode.HandleInput(mKeyboard, pCamera ,deltaTime);
	}

	void InputManager::PickMesh(const int screenX, const int screenY, const int wndWidth, const int wndHeight)
	{
		Ray ray = CreateMouseRay(screenX, screenY, wndWidth, wndHeight);
		float closest = FLT_MAX;

		PBRMesh* pickedMesh = nullptr;
		PBRSubMesh* pickedSubMesh = nullptr;
		DX11Renderer* renderer = dynamic_cast<DX11Renderer*>(pRenderer);

		// opaque meshes
		for (const auto& mesh : renderer->mMeshManager.meshes)
		{
			XMMATRIX worldMatrix = mesh->transform.GetModelMartix();

			for (auto& sub : mesh->subMeshes)
			{
				XMVECTOR minV = XMVector3TransformCoord(XMLoadFloat3(&sub.aabb.min), worldMatrix);
				XMVECTOR maxV = XMVector3TransformCoord(XMLoadFloat3(&sub.aabb.max), worldMatrix);

				XMFLOAT3 worldMin, worldMax;

				XMStoreFloat3(&worldMin, XMVectorMin(minV, maxV));
				XMStoreFloat3(&worldMax, XMVectorMax(minV, maxV));

				float t;

				if (renderer->mMeshManager.RayAABBIntersect(ray, worldMin, worldMax, t))
				{
					if (t < closest)
					{
						closest = t;
						pickedMesh = mesh.get();
						pickedSubMesh = &sub;
					}
				}
			}
		}

		// transparent meshes
		for (const auto& mesh : renderer->mMeshManager.meshesTransparent)
		{
			XMMATRIX worldMatrix = mesh->transform.GetModelMartix();

			for (auto& sub : mesh->subMeshes)
			{
				XMVECTOR minV = XMVector3TransformCoord(XMLoadFloat3(&sub.aabb.min), worldMatrix);
				XMVECTOR maxV = XMVector3TransformCoord(XMLoadFloat3(&sub.aabb.max), worldMatrix);

				XMFLOAT3 worldMin, worldMax;

				XMStoreFloat3(&worldMin, XMVectorMin(minV, maxV));
				XMStoreFloat3(&worldMax, XMVectorMax(minV, maxV));

				float t;

				if (renderer->mMeshManager.RayAABBIntersect(ray, worldMin, worldMax, t))
				{
					if (t < closest)
					{
						closest = t;
						pickedMesh = mesh.get();
						pickedSubMesh = &sub;
					}
				}
			}
		}

		if (pickedMesh && pickedSubMesh)
		{
			std::cout << "selected mesh: " << pickedMesh->modelPath << "\n";
			std::cout << "selected subMesh: " << pickedSubMesh->material.name << "\n";
		} else std::cout << "empty space selected" << "\n";
	}

	Ray InputManager::CreateMouseRay(const int screenX, const int screenY, const int wndWidth, const int wndHeight)
	{
		float ndcX = (2.0f * screenX) / wndWidth - 1.0f;
		float ndcY = 1.0f - (2.0f * screenY) / wndHeight;

		XMMATRIX invViewProj = XMMatrixInverse(nullptr, pCamera->GetViewMatrix() * pCamera->GetProjMatrix());

		XMVECTOR nearPoint = XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
		XMVECTOR farPoint = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

		nearPoint = XMVector3TransformCoord(nearPoint, invViewProj);
		farPoint = XMVector3TransformCoord(farPoint, invViewProj);

		Ray ray;

		ray.origin = nearPoint;
		ray.direction = XMVector3Normalize(farPoint - nearPoint);

		return ray;
	}

	void EditorInputMode::HandleInput(Keyboard& keyboard, Camera* pCamera, float& deltaTime)
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

	void GameInputMode::HandleInput(Keyboard& keyboard, Camera* pCamera, float& deltaTime)
	{

	}
}