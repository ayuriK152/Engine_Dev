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
	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }

private:
	void LoadBoneData(UINT64 boneByteSize, vector<XMFLOAT4X4>& boneTransforms);
	void CreateBoneSRV(vector<XMFLOAT4X4>& boneTransforms);

private:
	shared_ptr<Mesh> _mesh;

	UINT _boneSrvHeapIndex = 0;
	ComPtr<ID3D12Resource> _boneTransformBuffer = nullptr;
};

