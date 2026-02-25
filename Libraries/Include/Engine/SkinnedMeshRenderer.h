#pragma once

class Skeleton;

class SkinnedMeshRenderer : public MeshRenderer
{ 
	using Super = MeshRenderer;

public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

	void Init() override;
	void Render(ID3D12GraphicsCommandList* cmdList, UINT renderState) override;

public:
	void SetBoneData(const string& boneFileName) { _boneFileName = boneFileName; }
	void SetRootBone(const shared_ptr<Transform> rootBone);
	shared_ptr<Transform> GetRootBone() const { return _rootBone; }

private:
	shared_ptr<Transform> _rootBone;

	shared_ptr<Skeleton> _skeleton;
	string _boneFileName;
};

