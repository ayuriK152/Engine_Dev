#pragma once
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

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
#include "d3d12.h"
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/ResourceUploadBatch.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"
#include "DirectXTK/CommonStates.h"
using namespace DirectX;
using namespace Microsoft::WRL;

// External Libararies
#include "magic_enum/magic_enum.hpp"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "DirectXTK/DirectXTK12.lib")

#include "Descriptors.h"
#include "DataStructures.h"
#include "UniversalUtils.h"
#include "MathHelper.h"
#include "DebugHelper.h"
#include "DXUtil.h"
#include "GeometryGenerator.h"

#include "IExecute.h"
#include "Component.h"

#include "Resource.h"
#include "GameObject.h"
#include "Geometry.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "Rigidbody.h"

#include "UploadBuffer.h"
#include "FrameResource.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "InputManager.h"

#include "GameTimer.h"
#include "GameApplication.h"
#include "Graphic.h"