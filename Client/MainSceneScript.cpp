#include "pch.h"
#include "MainSceneScript.h"

REGISTER_COMPONENT(MainSceneScript)

MainSceneScript::~MainSceneScript()
{

}

void MainSceneScript::Init()
{
	_states.push_back(new FadeIn());
	_states.push_back(new Common());
	_states.push_back(new FadeOut());
	SetState(MainSceneState::FadeIn);

	_fadePanel = UI->CreateUI<UIPanel>();
	_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	_fadePanel->SetPassthroughMouse(true);
	_fadePanel->GetTransform()->SetStretchSize(true);
	_fadePanel->GetTransform()->SetSize({ 1.0f, 1.0f });
}

void MainSceneScript::Update()
{
	if (_isStateChanged) {
		_states[static_cast<int>(_currentState)]->StateStart(this);
		_isStateChanged = false;
	}

	_states[static_cast<int>(_currentState)]->StateUpdate(this);
}

void MainSceneScript::OnDestroy()
{

}

void MainSceneScript::LoadXML(Bulb::XMLElement compElem)
{

}

void MainSceneScript::SaveXML(Bulb::XMLElement compElem)
{

}

ComponentSnapshot MainSceneScript::CaptureSnapshot()
{
	ComponentSnapshot snapshot;
	snapshot.id = _id;
	snapshot.componentType = "MainSceneScript";

	return snapshot;
}

void MainSceneScript::RestoreSnapshot(ComponentSnapshot snapshot)
{
	SetState(MainSceneState::FadeIn);
	_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
}

void MainSceneScript::FadeIn::StateStart(MainSceneScript* owner)
{
	_elapsedTime = 0.0f;
	owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
}

void MainSceneScript::FadeIn::StateUpdate(MainSceneScript* owner)
{
	_elapsedTime += TIME->DeltaTime();
	if (_elapsedTime >= owner->_fadeInTime) {
		owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		owner->SetState(MainSceneState::Common);
	}
	else {
		owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f - _elapsedTime / owner->_fadeInTime });
	}
}

void MainSceneScript::FadeOut::StateStart(MainSceneScript* owner)
{
	_elapsedTime = 0.0f;
	owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
}

void MainSceneScript::FadeOut::StateUpdate(MainSceneScript* owner)
{
	_elapsedTime += TIME->DeltaTime();
	if (_elapsedTime >= owner->_fadeInTime) {
		owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		// 사망시 FadeOut, 다시 씬을 로드하는 부분 필요
		// owner->SetState(MainSceneState::Common);
	}
	else {
		owner->_fadePanel->SetColor({ 0.0f, 0.0f, 0.0f, _elapsedTime / owner->_fadeOutTime });
	}
}
