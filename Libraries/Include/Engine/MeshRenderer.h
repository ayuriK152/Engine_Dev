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

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;

	unique_ptr<UploadBuffer<MaterialConstants>> _materialCB = nullptr;
};

