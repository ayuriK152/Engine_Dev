#include "pch.h"
#include "Animator.h"

Animator::Animator() : Component(ComponentType::Animator)
{
	_isPlayOnInit = true;
	_isPlaying = true;
	_isLoop = true;
	_isCurrentAnimationEnd = false;

	_currentAnimation = EMPTY_ANIMATION;
	_nextAnimation = EMPTY_ANIMATION;
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
	if (_currentAnimation == EMPTY_ANIMATION || !_isPlaying)
		return;

	UpdateBoneTransform();

	if (_isInTransition) {
		_transitionElapsedTime += TIME->DeltaTime();
		if (_transitionElapsedTime >= _transitionTime) {
			_isInTransition = false;
			_currentAnimation = _nextAnimation;
			_nextAnimation = EMPTY_ANIMATION;
			_currentTick = _transitionTick;
			_transitionTick = 0.0f;
			_transitionElapsedTime = 0.0f;
		}
		else {
			float ticksPerSecond = _animations[_nextAnimation]->GetTicksPerSecond();
			_transitionTick += TIME->DeltaTime() * ticksPerSecond * _animationSpeed;
		}
	}

	float ticksPerSecond = (GetCurrentAnimation()->GetTicksPerSecond() != 0.0f) ? GetCurrentAnimation()->GetTicksPerSecond() : 25.0f;
	_currentTick += TIME->DeltaTime() * ticksPerSecond * _animationSpeed;

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
	if (_currentAnimation == EMPTY_ANIMATION)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = true;
	_isCurrentAnimationEnd = false;
}

void Animator::PauseAnimation()
{
	if (_currentAnimation == EMPTY_ANIMATION)
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
		Animation::KeyFrame currKeyFrame = GetCurrentAnimation()->Interpolate(objName, _currentTick, _lastKeyframeIndex[i]);

		if (currKeyFrame.tick < 0.0)
			continue;

		if (_isInTransition && _animations.contains(_nextAnimation))
		{
			// 다음 애니메이션 키프레임
			int dummyIdx = 0;
			Animation::KeyFrame nextFrame = _animations[_nextAnimation]->Interpolate(objName, _transitionTick, dummyIdx);

			// 가중치 계산
			float alpha = _transitionElapsedTime / _transitionTime;
			alpha = std::clamp(alpha, 0.0f, 1.0f);

			// Lerp / Slerp
			Vector3 pos = currKeyFrame.position.Lerp(nextFrame.position, alpha);
			Vector3 scale = currKeyFrame.scale.Lerp(nextFrame.scale, alpha);
			Vector4 rot;
			XMStoreFloat4(&rot, XMQuaternionSlerp(XMLoadFloat4(&currKeyFrame.rotation), XMLoadFloat4(&nextFrame.rotation), alpha));

			_childs[i]->SetLocalPosition(pos);
			_childs[i]->SetLocalScale(scale);
			_childs[i]->SetLocalQuaternion(rot);
		}
		else
		{
			_childs[i]->SetLocalScale(currKeyFrame.scale);
			_childs[i]->SetLocalQuaternion(currKeyFrame.rotation);
			_childs[i]->SetLocalPosition(currKeyFrame.position);
		}
	}
}

void Animator::SetCurrentAnimation(const string& animationName, float _transitionTime)
{
	if (_currentAnimation == EMPTY_ANIMATION || _transitionTime == 0.0f)
	{
		_currentAnimation = animationName;
		_currentTick = 0.0f;
		return;
	}
	_nextAnimation = animationName;
	_transitionTick = 0.0f;
	_isInTransition = true;
}
