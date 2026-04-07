#pragma once

class Animator;

class AnimationManager
{
	friend class BulbApplication;

private:
	AnimationManager() = default;
	~AnimationManager();

	void Update();

public:
	AnimationManager(const AnimationManager& rhs) = delete;
	AnimationManager& operator=(const AnimationManager& rhs) = delete;

	static AnimationManager* GetInstance();
	static Bulb::ProcessResult Delete();

	void AddAnimator(shared_ptr<Animator> animator) { _animators.push_back(animator); }
	void DeleteAnimator(shared_ptr<Animator> animator);

	void InitializeOnRuntime();

private:
	static AnimationManager* s_instance;

	vector<shared_ptr<Animator>> _animators;

	// 멀티스레드용
	vector<future<void>> _futures;
};

