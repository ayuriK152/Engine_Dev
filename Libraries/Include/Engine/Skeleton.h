#pragma once

class Skeleton
{
public:
	friend class SkeletonManager;
	Skeleton();
	~Skeleton();

public:
	void Init();

	void UpdateUploadBuffer();
	void UpdateBoneTransform(int id, XMMATRIX localMat);
	void UpdateBoneTransform(int id);
	void SetBone(map<string, BoneData> bones);
	void MapTransform(shared_ptr<Transform> transform);

	UINT GetBoneTransformSRVIndex() { return _boneTransformSrvIndex; }

private:
	void CreateBuffer();

public:
	vector<shared_ptr<Transform>> instancedTransforms;
	vector<vector<int>> depthSortedTransformIndices;

private:
	vector<BoneData> _bones;
	vector<XMFLOAT4X4> _finalMatrices;

	unique_ptr<UploadBuffer<XMFLOAT4X4>> _boneTransformUploadBuffer = nullptr;
	UINT _boneTransformSrvIndex = -1;
};

