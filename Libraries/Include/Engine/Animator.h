#pragma once
#define EMPTY_CURRENT_ANIMATION		"empty"

class Animation;

struct AnimationState
{
	string StateName;
	map<string, float> TransitionMap;	// StateName, fade
};

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

	void UpdateChildList();
	void UpdateBoneTransform();

	shared_ptr<Animation> GetCurrentAnimation() { return _currentAnimation != EMPTY_CURRENT_ANIMATION ? _animations[_currentAnimation] : nullptr; }
	void SetCurrentAnimation(const string& animationName) { _currentAnimation = animationName; }
	
	const map<string, shared_ptr<Animation>>& GetAnimations() { return _animations; }
	void AddAnimation(shared_ptr<Animation> animation) { 
		_animations[animation->GetName()] = animation;
		if (_currentAnimation == EMPTY_CURRENT_ANIMATION) {
			_currentAnimation = animation->GetName();
		}
	}
	void RemoveAnimation(shared_ptr<Animation> animation) { 
		RemoveAnimation(animation->GetName());
	}
	void RemoveAnimation(const string& animationName) { 
		if (_animations.find(animationName) != _animations.end()) {
			_animations.erase(animationName);
			if (_currentAnimation == animationName) {
				_currentAnimation = EMPTY_CURRENT_ANIMATION;
				_currentTick = 0.0f;
			}
		}
	}

private:
	float _currentTick = 0.0f;
	bool _isPlayOnInit;
	bool _isPlaying;
	bool _isLoop;

	map<string, shared_ptr<Animation>> _animations;
	string _currentAnimation;

	vector<shared_ptr<Transform>> _childs;
};

