#include "pch.h"
#include "StartMenuSceneScript.h"

REGISTER_COMPONENT(StartMenuSceneScript)

StartMenuSceneScript::~StartMenuSceneScript()
{

}

void StartMenuSceneScript::Init()
{
	_states.push_back(new MenuFadeIn());
	_states.push_back(new Menu());
	SetState(StartMenuSceneState::MenuFadeIn);

	_fadePanel = UI->CreateUI<UIPanel>();
	_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	_fadePanel->SetPassthroughMouse(true);
	_fadePanel->GetTransform()->SetStretchSize(true);
	_fadePanel->GetTransform()->SetSize({ 1.0f, 1.0f });

	_startButton = UI->CreateUI<UIButton>();
	_startButton->GetTransform()->SetDepth(3.0f);
	_startButton->GetTransform()->SetPosition({ 0.0f, -100.0f, 0.0f });
	_startButton->text->SetText("Start Game");
	_startButton->buttonEvent += [this]() {
		OnClickedStartButton();
	};

	shared_ptr<UIPanel> mainTitlePanel = UI->CreateUI<UIPanel>();
	mainTitlePanel->SetTexture(L"..\\Resources\\Textures\\Logos\\MainTitle.png");
	mainTitlePanel->GetTransform()->SetDepth(5.0f);
	mainTitlePanel->GetTransform()->SetSize({ 1500.0f, 468.75f });
	mainTitlePanel->GetTransform()->SetPosition({ 0.0f, 150.0f, 0.0f });

	SOUND->SetMasterVolume(0.3f);
}

void StartMenuSceneScript::Update()
{
	if (_isStateChanged) {
		_states[static_cast<int>(_currentState)]->StateStart(this);
		_isStateChanged = false;
	}

	_states[static_cast<int>(_currentState)]->StateUpdate(this);
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

void StartMenuSceneScript::MenuFadeIn::StateStart(StartMenuSceneScript* owner)
{
	_elapsedTime = 0;
}

void StartMenuSceneScript::MenuFadeIn::StateUpdate(StartMenuSceneScript* owner)
{
	_elapsedTime += TIME->DeltaTime();
	if (_elapsedTime >= owner->_fadeInTime) {
		owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		owner->SetState(StartMenuSceneState::Menu);
	}
	else {
		owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f - _elapsedTime / owner->_fadeInTime });
	}
}

void StartMenuSceneScript::Menu::StateStart(StartMenuSceneScript* owner)
{
	SOUND->LoadSound("Sounds/MainMenu.mp3", true);
	SOUND->PlaySound("Sounds/MainMenu.mp3");
}

void StartMenuSceneScript::Menu::StateUpdate(StartMenuSceneScript* owner)
{

}
