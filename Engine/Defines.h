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
#define DATA		GET_SINGLE(DataParseManager)


/* 
* 경로 define 명명 규칙
* 
* 1. 무엇에 대한 경로인지 + PATH
* 2. 하위 분류 이름
* 3. wstring인 경우 + W
*/
#define RESOURCE_PATH				"..\\Resources\\"
#define RESOURCE_PATHW				L"..\\Resources\\"
#define RESOURCE_PATH_MATERIAL		"..\\Resources\\Materials\\"
#define RESOURCE_PATH_MATERIALW		L"..\\Resources\\Materials\\"
#define RESOURCE_PATH_SHADER		"..\\Resources\\Shaders\\"
#define RESOURCE_PATH_SHADERW		L"..\\Resources\\Shaders\\"
#define RESOURCE_PATH_TEXTURE		"..\\Resources\\Textures\\"
#define RESOURCE_PATH_TEXTUREW		L"..\\Resources\\Textures\\"
#define RESOURCE_PATH_MESH			"..\\Resources\\Meshes\\"
#define RESOURCE_PATH_MESHW			L"..\\Resources\\Meshes\\"