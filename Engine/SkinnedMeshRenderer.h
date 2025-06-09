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
	void SetBoneData(const map<string, shared_ptr<Bone>>& bones) { _bones = bones; }
	void SetRootBone(const shared_ptr<Transform> rootBone);
	shared_ptr<Transform> GetRootBone() const { return _rootBone; }

	vector<shared_ptr<Transform>> GetBoneTransforms() const { return _boneTransforms; }

private:
	void CreateBoneSRV(vector<shared_ptr<Transform>>& boneTransforms);

public:

private:
	UINT _boneSrvHeapIndex = 0;

	shared_ptr<Transform> _rootBone;
	vector<XMFLOAT4X4> _boneOffsetTransforms;
	vector<shared_ptr<Transform>> _boneTransforms;
	map<string, shared_ptr<Bone>> _bones;
	unique_ptr<UploadBuffer<XMFLOAT4X4>> _boneTransformUploadBuffer = nullptr;
};

