#include "pch.h"
#include "AnimationManager.h"

AnimationManager* AnimationManager::s_instance = nullptr;

AnimationManager::~AnimationManager()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - AnimationManager\n";
#endif

	for (shared_ptr<Animator> a : _animators)
		a.reset();
}

AnimationManager* AnimationManager::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new AnimationManager();
	return s_instance;
}

Bulb::ProcessResult AnimationManager::Delete()
{
	if (s_instance != nullptr) {
		delete s_instance;
		s_instance = nullptr;
		return Bulb::ProcessResult::SUCCESS;
	}
	return Bulb::ProcessResult::FAILED_INSTANCE_NOT_FOUND;
}

void AnimationManager::Update()
{
	_futures.clear();

	for (auto& animator : _animators) {
		if (!animator->GetGameObject()->IsActive()) continue;
		_futures.push_back(THREAD->EnqueueJob([animator] { animator->Update(); }));
	}

	for (auto& f : _futures)
		f.get();
}

void AnimationManager::DeleteAnimator(shared_ptr<Animator> animator)
{
	for (int i = 0; i < _animators.size(); ++i) {
		if (_animators[i] == animator) {
			_animators.erase(_animators.begin() + i);
			break;
		}
	}
}

void AnimationManager::InitializeOnRuntime()
{
	_animators.clear();
}
