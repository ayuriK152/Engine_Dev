#pragma once
#define EMPTY_CURRENT_ANIMATION		"empty"

class Animation;

class Animator : public Component
{
public:
	Animator();
	virtual ~Animator();

	void Init() override;
	void Update() override;

public:
	float GetCurrentTick() { return _currentTick; }

	bool IsPlayOnInit() { return _isPlayOnInit; }
	void SetPlayOnInit(bool value) { _isPlayOnInit = value; }

	bool IsPlaying() { return _isPlaying; }
	void PlayAnimation();
	void PauseAnimation();

	bool IsLoop() { return _isLoop; }
	void SetLoop(bool loop) { _isLoop = loop; }

	void UpdateBoneTransform();

	shared_ptr<Animation> GetCurrentAnimation() { return _animations[_currentAnimation]; }
	void SetCurrentAnimation(const string& animationName) { _currentAnimation = animationName; }
	
	const map<string, shared_ptr<Animation>>& GetAnimations() { return _animations; }
	void AddAnimation(shared_ptr<Animation> animation) { 
		_animations[animation->GetName()] = animation;
		if (_currentAnimation == EMPTY_CURRENT_ANIMATION) {
			_currentAnimation = animation->GetName();
		}
	}

private:
	float _currentTick = 0.0f;
	bool _isPlayOnInit = true;
	bool _isPlaying = true;
	bool _isLoop = true;

	map<string, shared_ptr<Animation>> _animations;
	string _currentAnimation;
};

