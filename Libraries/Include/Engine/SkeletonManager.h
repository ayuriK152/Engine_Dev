#pragma once

class Skeleton;

class BULB_API SkeletonManager
{
	friend class BulbApplication;

private:
	SkeletonManager() = default;
	~SkeletonManager();

public:
	SkeletonManager(const SkeletonManager& rhs) = delete;
	SkeletonManager& operator=(const SkeletonManager& rhs) = delete;

	static SkeletonManager* GetInstance();
	static Bulb::ProcessResult Delete();

	shared_ptr<Skeleton> LoadSkeleton(string fileName, shared_ptr<Transform> rootTransform);
	shared_ptr<Skeleton> GetSkeleton(shared_ptr<Transform> rootTransform);

private:
	static SkeletonManager* s_instance;

	vector<shared_ptr<Skeleton>> _instancedSkeletons;
};

