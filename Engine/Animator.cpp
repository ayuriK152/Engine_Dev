#include "pch.h"
#include "Animator.h"

Animator::Animator() : Component(ComponentType::Animator)
{
	_isPlayOnInit = true;
	_isPlaying = true;
	_isLoop = true;
	_isCurrentAnimationEnd = false;

	_currentAnimation = EMPTY_CURRENT_ANIMATION;
}

Animator::~Animator()
{

}

void Animator::Init()
{
	_isPlaying = _isPlayOnInit;
	UpdateChildList();
}

void Animator::Update()
{
	if (_currentAnimation == EMPTY_CURRENT_ANIMATION || !_isPlaying)
		return;

	UpdateBoneTransform();

	float ticksPerSecond = (GetCurrentAnimation()->GetTicksPerSecond() != 0.0f) ? GetCurrentAnimation()->GetTicksPerSecond() : 25.0f;
	_currentTick += TIME->DeltaTime() * ticksPerSecond;

	if (_currentTick > GetCurrentAnimation()->GetDuration()) {
		if (_isLoop)
			_currentTick = 0.0f;
		else
		{
			_isCurrentAnimationEnd = true;
			_currentTick = GetCurrentAnimation()->GetDuration();
		}
	}
	else
	{
		_isCurrentAnimationEnd = false;
	}
}

void Animator::PlayAnimation()
{
	if (_currentAnimation == EMPTY_CURRENT_ANIMATION)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = true;
	_isCurrentAnimationEnd = false;
}

void Animator::PauseAnimation()
{
	if (_currentAnimation == EMPTY_CURRENT_ANIMATION)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = false;
}

void Animator::UpdateChildList()
{
	_childs = GetTransform()->GetChilds();
	for (int i = 0; i < _childs.size(); i++)
	{
		_childs.insert(_childs.end(), _childs[i]->GetChilds().begin(), _childs[i]->GetChilds().end());
	}
	_lastKeyframeIndex.clear();
	_lastKeyframeIndex.resize(_childs.size(), 0);
}

void Animator::UpdateBoneTransform()
{
	for (int i = 0; i < _childs.size(); i++)
	{
		string objName = _childs[i]->GetGameObject()->GetName();
		Animation::KeyFrame keyFrame = GetCurrentAnimation()->Interpolate(objName, _currentTick, _lastKeyframeIndex[i]);

		if (keyFrame.tick < 0.0)
			continue;

		_childs[i]->SetLocalScale(keyFrame.scale);
		_childs[i]->SetLocalQuaternion(keyFrame.rotation);
		_childs[i]->SetLocalPosition(keyFrame.position);
	}
}

void Animator::SetCurrentAnimation(const string& animationName)
{
	_currentAnimation = animationName;
	_currentTick = 0.0f;
}
