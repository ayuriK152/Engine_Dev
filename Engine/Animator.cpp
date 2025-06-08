#include "pch.h"
#include "Animator.h"

Animator::Animator() : Component(ComponentType::Animator)
{

}

Animator::~Animator()
{

}

void Animator::Update()
{
	if (_currentAnimation == nullptr)
		return;

	float ticksPerSecond = (_currentAnimation->GetTicksPerSecond() != 0.0f) ? _currentAnimation->GetTicksPerSecond() : 25.0f;
	_currentTime += TIME->DeltaTime() * ticksPerSecond;

	if (_currentTime > _currentAnimation->GetDuration()) {
		if (_isLoop)
			_currentTime = fmod(_currentTime, _currentAnimation->GetDuration());
		else
			_currentTime = _currentAnimation->GetDuration();
	}
}
