#include "pch.h"
#include "SkeletonManager.h"

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
