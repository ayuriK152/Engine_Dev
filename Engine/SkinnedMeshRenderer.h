#pragma once

class SkinnedMeshRenderer : public MeshRenderer
{ 
	using Super = MeshRenderer;

public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

	void Init() override;
	void Update() override;
	void Render() override;

public:
	void UpdateBoneTransforms(const shared_ptr<Transform> root);
	void SetBoneData(const map<string, Bone>& bones);
	void SetRootBone(const shared_ptr<Transform> rootBone);
	shared_ptr<Transform> GetRootBone() const { return _rootBone; }

	vector<shared_ptr<Transform>> GetBoneTransforms() const { return _boneTransforms; }

private:
	void CreateBoneSRV(vector<shared_ptr<Transform>>& boneTransforms);

public:

private:
	UINT _boneTransformSrvIndex = 0;
	UINT _boneOffsetSrvIndex = 0;

	shared_ptr<Transform> _rootBone;
	vector<shared_ptr<Transform>> _boneTransforms;
	XMFLOAT4X4 _boneOffsets[MAX_BONE_COUNT];
	XMFLOAT4X4 _finalMatrices[MAX_BONE_COUNT];
	unique_ptr<UploadBuffer<XMFLOAT4X4>> _boneTransformUploadBuffer = nullptr;
};

