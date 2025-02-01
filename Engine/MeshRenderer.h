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

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
};

