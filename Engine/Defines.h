#pragma once

#define DECLARE_SINGLE(classname)		\
private:								\
	classname() { }						\
public:									\
	static classname* GetInstance()		\
	{									\
		static classname s_instance;	\
		return &s_instance;				\
	}									\

#define GET_SINGLE(classname)	classname::GetInstance()

#ifndef ReleaseCom
#define ReleaseCom(x) { if (x) { x->Release(); x = 0; } }
#endif

#define GAMEAPP		GET_SINGLE(GameApplication)
#define GRAPHIC		GET_SINGLE(Graphic)
#define TIME		GET_SINGLE(GameTimer)
#define RESOURCE	GET_SINGLE(ResourceManager)
#define RENDER		GET_SINGLE(RenderManager)
#define INPUTM		GET_SINGLE(InputManager)
#define ENGINEGUI	GET_SINGLE(EngineGUIManager)
#define ENGINESTAT	GET_SINGLE(EngineStatusManager)
#define DEBUG		GET_SINGLE(DebugManager)
#define PHYSICS		GET_SINGLE(PhysicsManager)
#define PARTICLE	GET_SINGLE(ParticleManager)
#define SKELETON	GET_SINGLE(SkeletonManager)
#define THREAD		GET_SINGLE(ThreadManager)

#define FILEIO		GET_SINGLE(FileIOUtil)


/**************/
/* Extentions */
/**************/
#define BULB_EXT_UNKNOWN			".brsc"
#define BULB_EXT_UNKNOWNW			L".brsc"
#define BULB_EXT_MESH				".bmesh"
#define BULB_EXT_MESHW				L".bmesh"
#define BULB_EXT_ANIMATION			".banim"
#define BULB_EXT_ANIMATIONW			L".banim"
#define BULB_EXT_BONE				".bbone"
#define BULB_EXT_BONEW				L".bbone"
#define BULB_EXT_PREFAB				".bprefab"
#define BULB_EXT_PREFABW			L".bprefab"


/*********/
/* Paths */
/*********/
#define RESOURCE_PATH					"..\\Resources\\"
#define RESOURCE_PATHW					L"..\\Resources\\"
#define RESOURCE_PATH_MATERIAL			"..\\Resources\\Materials\\"
#define RESOURCE_PATH_MATERIALW			L"..\\Resources\\Materials\\"
#define RESOURCE_PATH_SHADER			"..\\Resources\\Shaders\\"
#define RESOURCE_PATH_SHADERW			L"..\\Resources\\Shaders\\"
#define RESOURCE_PATH_TEXTURE			"..\\Resources\\Textures\\"
#define RESOURCE_PATH_TEXTUREW			L"..\\Resources\\Textures\\"
#define RESOURCE_PATH_MESH				"..\\Resources\\Meshes\\"
#define RESOURCE_PATH_MESHW				L"..\\Resources\\Meshes\\"
#define RESOURCE_PATH_ANIMATION			"..\\Resources\\Animations\\"
#define RESOURCE_PATH_ANIMATIONW		L"..\\Resources\\Animations\\"
#define RESOURCE_PATH_BONE				"..\\Resources\\Bones\\"
#define RESOURCE_PATH_BONEW				L"..\\Resources\\Bones\\"
#define RESOURCE_PATH_PREFAB			"..\\Resources\\Prefabs\\"
#define RESOURCE_PATH_PREFABW			L"..\\Resources\\Prefabs\\"
#define RESOURCE_PATH_ASSET				"..\\Resources\\Assets\\"
#define RESOURCE_PATH_ASSETW			L"..\\Resources\\Assets\\"