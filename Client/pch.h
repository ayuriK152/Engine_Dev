#pragma once

#include "Engine/pch.h"
#ifdef BULB_EDITOR
#ifdef _DEBUG
#pragma comment(lib, "Engine/Debug/BulbEditorCore.lib")
#else
#pragma comment(lib, "Engine/Release/BulbEditorCore.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "Engine/Debug/BulbClientCore.lib")
#else
#pragma comment(lib, "Engine/Release/BulbClientCore.lib")
#endif
#endif