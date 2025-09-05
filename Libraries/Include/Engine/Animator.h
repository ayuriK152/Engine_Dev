#pragma once
#define EMPTY_ANIMATION		"empty"

class Animation;

struct AnimationState
{
	string StateName;
	map<string, float> TransitionMap;	// StateName, fade
};

// 일단은 애니메이션 속도 조절만
struct AnimationEvent
{
	float Tick;
	float Speed;
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
	bool IsCurrentAnimationEnd() { return _isCurrentAnimationEnd; }

	bool IsLoop() { return _isLoop; }
	void SetLoop(bool loop) { _isLoop = loop; }

	void UpdateChildList();
	void UpdateBoneTransform();

	shared_ptr<Animation> GetCurrentAnimation() { return _currentAnimation != EMPTY_ANIMATION ? _animations[_currentAnimation] : nullptr; }
	void SetCurrentAnimation(const string& animationName, float transitionTime = 0.1f);
	
	const map<string, shared_ptr<Animation>>& GetAnimations() { return _animations; }
	void AddAnimation(shared_ptr<Animation> animation);
	void RemoveAnimation(shared_ptr<Animation> animation) { 
		RemoveAnimation(animation->GetName());
	}
	void RemoveAnimation(const string& animationName) { 
		if (_animations.find(animationName) != _animations.end()) {
			_animations.erase(animationName);
			if (_currentAnimation == animationName) {
				_currentAnimation = EMPTY_ANIMATION;
				_currentTick = 0.0f;
			}
		}
	}

	void UpdateAnimationEvent();
	void LoadAnimationEvents(const string& path);

	shared_ptr<Animation> GetPreviewAnimation() { return _previewAnimation; }
	void SetPreviewAnimation(shared_ptr<Animation> previewAnimation) {
		_previewAnimation = previewAnimation;
		_previewTick = 0.0f;
	}
	bool IsPreviewMode() { return _isPreviewMode; }
	void SetPreviewMode(bool value);
	bool IsPreviewPlaying() { return _isPreviewPlaying; }
	void SetPreviewPlaying(bool value) { _isPreviewPlaying = value; }
	float GetPreviewTick() { return _previewTick; }
	void SetPreviewTick(float value) { 
		_previewTick = value; 
		UpdateBoneTransform();
	}

private:
	bool _isPlayOnInit;
	bool _isPlaying;
	bool _isCurrentAnimationEnd;		// 콜백 방식으로 바꾸는거 고려.
	bool _isLoop;
	bool _isPreviewMode = false;

	float _currentTick = 0.0f;
	float _transitionTick = 0.0f;
	float _transitionTime = 0.1f;
	float _transitionElapsedTime = 0.0f;
	bool _isInTransition = false;

	map<string, shared_ptr<Animation>> _animations;

	string _currentAnimation;
	string _nextAnimation;

	map<string, vector<AnimationEvent>> _animationEvents;
	float _currentAnimationSpeed = 1.0f;
	float _nextAnimationSpeed = 1.0f;
	int _currentAnimationEventIndex = 0;
	int _nextAnimationEventIndex = 0;

	// Preview Mode Stuffs
	shared_ptr<Animation> _previewAnimation;
	bool _isPreviewPlaying = false;
	float _previewTick = 0.0f;

	vector<shared_ptr<Transform>> _childs;
	vector<int> _lastKeyframeIndex;
};
