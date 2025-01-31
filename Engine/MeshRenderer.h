#pragma once
#include "Component.h"

class MeshRenderer : public Component
{
	using Super = Component;
public:
	MeshRenderer();
	~MeshRenderer();

	void Init() override;
	void Update() override;

	void SetMesh(shared_ptr<MeshData> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

private:
	shared_ptr<MeshData> _mesh;
	shared_ptr<Material> _material;
};

