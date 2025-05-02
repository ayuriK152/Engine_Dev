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