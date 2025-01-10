#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Types.h"
#include "Defines.h"

// STL
#include <memory>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
using namespace std;

// Win
#include <windows.h>
#include <assert.h>

// DX
#include "d3dx12/d3dx12.h"
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <DirectXMath.h>
using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")