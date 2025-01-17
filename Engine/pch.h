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
#include <windowsx.h>
#include <assert.h>
#include <comdef.h>

// DX
#include "d3dx12/d3dx12.h"
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#include "Descriptors.h"
#include "UniversalUtils.h"
#include "MathHelper.h"
#include "DXUtil.h"

#include "GameObject.h"
#include "Mesh.h"
#include "Vertex.h"

#include "GameTimer.h"
#include "UploadBuffer.h"
#include "FrameResource.h"
#include "GameApplication.h"
#include "Graphic.h"