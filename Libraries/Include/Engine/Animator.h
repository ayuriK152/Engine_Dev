#pragma once

class Animation;

class Animator : public Component
{
public:
	Animator();
	virtual ~Animator();

	void Update() override;

public:
	float GetCurrentTick() { return _currentTick; }

	void PlayAnimation();
	void PauseAnimation();

	bool IsLoop() { return _isLoop; }
	void SetLoop(bool loop) { _isLoop = loop; }

	void UpdateBoneTransform();

	shared_ptr<Animation> GetCurrentAnimation() { return _currentAnimation; }
	void AddAnimation(shared_ptr<Animation> animation) { 
		_animations[animation->GetName()] = animation;
		if (!_currentAnimation) {
			_currentAnimation = animation;
		}
	}

private:
	shared_ptr<Animation> _currentAnimation;

	map<string, shared_ptr<Animation>> _animations;
	float _currentTick = 0.0f;
	bool _isPlaying = true;
	bool _isLoop = true;
};

