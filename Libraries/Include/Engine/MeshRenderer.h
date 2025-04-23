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

	shared_ptr<Material> GetMaterial() { return _submeshMaterials[0]; }
	shared_ptr<Material> GetMaterial(int index) { return _submeshMaterials[index]; }
	void SetMaterial(shared_ptr<Material> mat) { _submeshMaterials[0] = mat; }
	void SetMaterial(shared_ptr<Material> mat, int index) { _submeshMaterials[index] = mat; }

private:
	void LoadBoneData(UINT64 boneByteSize, vector<XMFLOAT4X4>& boneTransforms);
	void CreateBoneSRV(vector<XMFLOAT4X4>& boneTransforms);

public:
	shared_ptr<Transform> rootBone;

private:
	shared_ptr<Mesh> _mesh;
	vector<shared_ptr<Material>> _submeshMaterials;

	UINT _boneSrvHeapIndex = 0;
	ComPtr<ID3D12Resource> _boneTransformBuffer = nullptr;
};

