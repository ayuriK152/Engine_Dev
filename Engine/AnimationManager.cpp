#include "pch.h"
#include "AnimationManager.h"

AnimationManager::~AnimationManager()
{

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
