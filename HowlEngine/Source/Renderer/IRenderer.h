#pragma once
#include "Windows.h"

class IRenderer
{
public:
	virtual ~IRenderer() {};
	virtual void Initialize(HWND hwnd, const UINT width, const UINT height) = 0;
	virtual void Update(const double deltaTime) = 0;
	virtual void Release() = 0;
};