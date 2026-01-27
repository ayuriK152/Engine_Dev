#pragma once

// Debuging Console
// #pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

// STL
#include <memory>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <algorithm>

// Win
#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <comdef.h>
#include <Shlwapi.h>
#include <thread>
#include <future>
#include <filesystem>
#include <fstream>

#pragma comment(lib, "Shlwapi.lib")

using namespace std;

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
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/ResourceUploadBatch.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"
#include "DirectXTK/CommonStates.h"

using namespace DirectX;
using namespace Microsoft::WRL;


/////////////////////////
// External Libararies //
/////////////////////////

#include "magic_enum/magic_enum.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "tinyxml2/tinyxml2.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_stdlib.h"
#include "ImGUI/imgui_impl_dx12.h"
#include "ImGUI/imgui_impl_win32.h"

// Jolt Physics
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
//#include "stb/stb_image.h"
//#include "stb/stb_image_write.h"

using namespace tinyxml2;
using namespace JPH;
using namespace JPH::literals;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/Debug/DirectXTK12.lib")
#pragma comment(lib, "Jolt/Debug/Jolt.lib")
#else
#pragma comment(lib, "DirectXTK/Release/DirectXTK12.lib")
#pragma comment(lib, "Jolt/Release/Jolt.lib")
#endif
#pragma comment(lib, "assimp/assimp-vc143-mtd.lib")

#include "Types.h"
#include "Defines.h"
#include "Descriptors.h"
#include "DataStructures.h"

#include "UniversalUtils.h"
#include "MathHelper.h"
#include "DXUtil.h"
#include "GeometryGenerator.h"

#include "Constants.h"

#include "IExecute.h"
#include "Component.h"

/*************/
/* Resources */
/*************/
#include "Resource.h"
#include "GameObject.h"
#include "Geometry.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Animation.h"

/**************/
/* Components */
/**************/
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Rigidbody.h"
#include "Animator.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "Script.h"
#include "ParticleEmitter.h"

#include "UploadBuffer.h"
#include "FrameResource.h"
#include "ShadowMap.h"

/*******/
/* ETC */
/*******/
#include "Skeleton.h"

/************/
/* Managers */
/************/
#include "ResourceManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "FileIOUtil.h"
#include "EngineGUIManager.h"
#include "EngineStatusManager.h"
#include "DebugManager.h"
#include "PhysicsManager.h"
#include "ParticleManager.h"
#include "SkeletonManager.h"
#include "ThreadManager.h"
#include "AnimationManager.h"

#include "GameTimer.h"
#include "GameApplication.h"
#include "Graphic.h"