#pragma once

class Animator;

class AnimationManager
{
	DECLARE_SINGLE(AnimationManager);
public:
	~AnimationManager();

	void Update();

public:
	void AddAnimator(shared_ptr<Animator> animator) { _animators.push_back(animator); }

private:
	vector<shared_ptr<Animator>> _animators;

	// 멀티스레드용
	vector<future<void>> _futures;
};

