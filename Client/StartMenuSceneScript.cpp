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
	_states.push_back(new MenuFadeOut());
	SetState(StartMenuSceneState::MenuFadeIn);

	_fadePanel = UI->CreateUI<UIPanel>();
	_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	_fadePanel->SetPassthroughMouse(true);
	_fadePanel->GetTransform()->SetStretchSize(true);
	_fadePanel->GetTransform()->SetSize({ 1.0f, 1.0f });

	_startButton = UI->CreateUI<UIButton>();
	_startButton->GetTransform()->SetDepth(3.0f);
	_startButton->GetTransform()->SetPosition({ 0.0f, -100.0f, 0.0f });
	_startButton->background->SetTexture(L"..\\Resources\\Textures\\UI\\ButtonHovered.png");
	_startButton->background->GetTransform()->SetSize({ 300.0f, 75.0f });
	_startButton->text->SetFont("Georgia");
	_startButton->text->SetText("Start Game");
	_startButton->mouseEnterEvent += [this]() { OnMouseEnterStartButton(); };
	_startButton->mouseDownEvent += [this]() { OnClickedStartButton(); };

	shared_ptr<UIPanel> mainTitlePanel = UI->CreateUI<UIPanel>();
	mainTitlePanel->SetTexture(L"..\\Resources\\Textures\\Logos\\MainTitle.png");
	mainTitlePanel->GetTransform()->SetDepth(5.0f);
	mainTitlePanel->GetTransform()->SetSize({ 1500.0f, 468.75f });
	mainTitlePanel->GetTransform()->SetPosition({ 0.0f, 150.0f, 0.0f });

	SOUND->SetMasterVolume(0.3f);

	SOUND->AddGroup("BGM");

	SOUND->LoadSound("Sounds/MainMenu.mp3", true);
	SOUND->LoadSound("Sounds/UIButtonHovered.mp3", false);
	SOUND->LoadSound("Sounds/StartGameButton.mp3", false);
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

ComponentSnapshot StartMenuSceneScript::CaptureSnapshot()
{
	ComponentSnapshot snapshot;
	snapshot.id = _id;
	snapshot.componentType = "StartMenuSceneScript";

	return snapshot;
}

void StartMenuSceneScript::RestoreSnapshot(ComponentSnapshot snapshot)
{
	SetState(StartMenuSceneState::MenuFadeIn);
	_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
}

void StartMenuSceneScript::OnMouseEnterStartButton()
{
	SOUND->PlaySound("Sounds/UIButtonHovered.mp3");
}

void StartMenuSceneScript::OnClickedStartButton()
{
	SOUND->PlaySound("Sounds/StartGameButton.mp3");
	SetState(StartMenuSceneState::MenuFadeOut);
}

void StartMenuSceneScript::MenuFadeIn::StateStart(StartMenuSceneScript* owner)
{
	_elapsedTime = 0.0f;
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
	SOUND->PlaySound("Sounds/MainMenu.mp3", "BGM");
}

void StartMenuSceneScript::MenuFadeOut::StateStart(StartMenuSceneScript* owner)
{
	_elapsedTime = 0.0f;
	owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
}

void StartMenuSceneScript::MenuFadeOut::StateUpdate(StartMenuSceneScript* owner)
{
	_elapsedTime += TIME->DeltaTime();
	if (_elapsedTime >= owner->_soundFadeOutTime) {
		SOUND->StopSoundGroup("BGM");
		SCENE->LoadSceneOnRender("MainScene.xml");
	}
	else {
		SOUND->SetVolume(1.0f - _elapsedTime / owner->_soundFadeOutTime, "BGM");
	}
}
