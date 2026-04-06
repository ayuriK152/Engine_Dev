#include "pch.h"
#include "SkeletonManager.h"

SkeletonManager* SkeletonManager::s_instance = nullptr;

SkeletonManager::~SkeletonManager()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - SkeletonManager\n";
#endif

	for (shared_ptr<Skeleton> s : _instancedSkeletons)
		s.reset();
}

SkeletonManager* SkeletonManager::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new SkeletonManager();
	return s_instance;
}

Bulb::ProcessResult SkeletonManager::Delete()
{
	if (s_instance != nullptr) {
		delete s_instance;
		s_instance = nullptr;
		return Bulb::ProcessResult::SUCCESS;
	}
	return Bulb::ProcessResult::FAILED_INSTANCE_NOT_FOUND;
}

shared_ptr<Skeleton> SkeletonManager::LoadSkeleton(string fileName, shared_ptr<Transform> rootTransform)
{
	shared_ptr<Skeleton> skeleton = make_shared<Skeleton>();
	skeleton->SetBone(RESOURCE->LoadBone(fileName));
	skeleton->MapTransform(rootTransform);
	_instancedSkeletons.push_back(skeleton);
	return skeleton;
}

shared_ptr<Skeleton> SkeletonManager::GetSkeleton(shared_ptr<Transform> rootTransform)
{
	for (auto& skeleton : _instancedSkeletons) {
		if (skeleton->instancedTransforms[0] == nullptr)
			continue;

		if (skeleton->instancedTransforms[0] == rootTransform)
			return skeleton;
	}

	return nullptr;
}
