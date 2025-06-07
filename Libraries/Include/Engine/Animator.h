#pragma once

class Animation;

class Animator : public Component
{
public:
	Animator();
	virtual ~Animator();

public:
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
};

