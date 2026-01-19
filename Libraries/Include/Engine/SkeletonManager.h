#pragma once

class Skeleton;

class SkeletonManager
{
	DECLARE_SINGLE(SkeletonManager);
public:
	shared_ptr<Skeleton> LoadSkeleton(string fileName, shared_ptr<Transform> rootTransform);
	shared_ptr<Skeleton> GetSkeleton(shared_ptr<Transform> rootTransform);

private:
	vector<shared_ptr<Skeleton>> _instancedSkeletons;
};

