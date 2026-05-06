#include "pch.h"
#include "StartMenuSceneScript.h"

REGISTER_COMPONENT(StartMenuSceneScript)

StartMenuSceneScript::~StartMenuSceneScript()
{

}

void StartMenuSceneScript::Init()
{
	_startButton = UI->CreateUI<UIButton>();
	_startButton->text->SetText("Start Game");
	_startButton->buttonEvent += [this]() {
		OnClickedStartButton();
	};
}

void StartMenuSceneScript::Update()
{

}

void StartMenuSceneScript::OnDestroy()
{

}

void StartMenuSceneScript::LoadXML(Bulb::XMLElement compElem)
{

}

void StartMenuSceneScript::SaveXML(Bulb::XMLElement compElem)
{

}

void StartMenuSceneScript::OnClickedStartButton()
{
	SCENE->LoadSceneOnRender("MainScene.xml");
}
