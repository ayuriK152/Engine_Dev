#include "pch.h"
#include "AnimationManager.h"

AnimationManager::~AnimationManager()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - AnimationManager\n";
#endif

	for (shared_ptr<Animator> a : _animators)
		a.reset();
}

void AnimationManager::Update()
{
	_futures.clear();

	for (auto& animator : _animators) {
		_futures.push_back(THREAD->EnqueueJob([animator] { animator->Update(); }));
	}

	for (auto& f : _futures)
		f.get();
}

void AnimationManager::InitializeOnRuntime()
{
	_animators.clear();
}
