#pragma once

#ifdef _DEBUG
#error "Asset parser does not support build on debug state"
#endif

#define JPH_DEBUG_RENDERER

#include "Engine/pch.h"
#pragma comment(lib, "Engine/Release/BulbEditorCore.lib")

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#pragma comment(lib, "assimp/assimp-vc143-mt.lib")

#include "AssetLoader.h"