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
	void UpdateBoneTransforms(const shared_ptr<Transform> root, vector<shared_ptr<Transform>>& boneTransforms);
	void SetBoneData(const map<string, shared_ptr<Bone>>& bones) { _bones = bones; }

private:
	void CreateBoneSRV(vector<shared_ptr<Transform>>& boneTransforms);

public:
	shared_ptr<Transform> rootBone;

private:
	UINT _boneSrvHeapIndex = 0;

	vector<XMFLOAT4X4> _boneOffsetTransforms;
	map<string, shared_ptr<Bone>> _bones;
	unique_ptr<UploadBuffer<XMFLOAT4X4>> _boneTransformTest = nullptr;
};

