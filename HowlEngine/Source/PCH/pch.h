#ifndef PCH_H
#define PCH_H

// standart
#include <iostream> 
#include <memory>
#include <vector>
#include <string>
#include "wrl.h"

// dx12
#include "d3d12.h"
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
// dx11
#include "d3d11.h"

// math
#include <DirectXMath.h>

// lib files
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

// internal
#include "../Config/EngineDLLConfig.h"
#include "../Config/EngineGlobalSettings.h"
#include "../Utils/Utils.h"

#endif 
