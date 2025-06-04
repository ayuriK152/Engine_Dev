#pragma once
#include "Component.h"
#include "UploadBuffer.h"

class MeshRenderer : public Component
{
	using Super = Component;
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void Init() override;
	void Update() override;
	void Render() override;

public:
	shared_ptr<Mesh> GetMesh() { return _mesh; }
	void SetMesh(shared_ptr<Mesh> mesh);

	shared_ptr<Material> GetMaterial() { return _material; }
	void SetMaterial(shared_ptr<Material> mat) { _material = mat; }

	void UpdateBoneTransforms(const shared_ptr<Transform> root, vector<shared_ptr<Transform>>& boneTransforms);
	void SetBoneData(const map<string, shared_ptr<Bone>>& bones) { _bones = bones; }

private:
	void CreateBoneSRV(vector<shared_ptr<Transform>>& boneTransforms);

public:
	shared_ptr<Transform> rootBone;

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;

	UINT _boneSrvHeapIndex = 0;

	vector<XMFLOAT4X4> _boneOffsetTransforms;
	map<string, shared_ptr<Bone>> _bones;
	unique_ptr<UploadBuffer<XMFLOAT4X4>> _boneTransformTest = nullptr;
};

