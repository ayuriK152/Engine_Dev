#pragma once
#include "Component.h"
#include "UploadBuffer.h"

class MeshRenderer : public Component
{
	using Super = Component;
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void Render(ID3D12GraphicsCommandList* cmdList) override;

protected:
	MeshRenderer(ComponentType type);

	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;

public:
	shared_ptr<Mesh> GetMesh() { return _mesh; }
	void SetMesh(shared_ptr<Mesh> mesh);

	shared_ptr<Material> GetMaterial() { return _material; }
	void SetMaterial(shared_ptr<Material> mat) { _material = mat; }
};

