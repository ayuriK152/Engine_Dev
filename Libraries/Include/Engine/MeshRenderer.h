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

	shared_ptr<Mesh> GetMesh() { return _mesh; }
	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }

private:
	shared_ptr<Mesh> _mesh;

	UINT _srvHeapIndex = 0;
	//unique_ptr<UploadBuffer<vector<XMFLOAT4X4>>> _boneTransformBuffer = nullptr;
	ComPtr<ID3D12Resource> _boneTransformBuffer = nullptr;
};

