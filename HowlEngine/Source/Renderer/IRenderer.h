#pragma once
#include "Windows.h"
#include "../Camera/Camera.h"

using namespace HEngine;

class IRenderer
{
public:
	virtual ~IRenderer() {};
	virtual void Initialize(HWND hwnd, const UINT width, const UINT height, Camera* pCamera) = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Release() = 0;
};