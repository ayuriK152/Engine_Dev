#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
	using Super = Collider;
public:
	BoxCollider();
	virtual ~BoxCollider();

	virtual void Init()override;
	virtual void FixedUpdate()override;
	virtual void Update()override;

public:
	virtual CollisionInfo CheckCollide(shared_ptr<Collider>& other)override;

	float CheckAxis(XMVECTOR& axis, XMMATRIX& rotA, XMMATRIX& rotB, BoundingOrientedBox& target);

	BoundingOrientedBox& GetBoundingBox() { return _boundingBox; }
	XMMATRIX& GetBoundingBoxRotationMatrix() { return XMMatrixRotationQuaternion(XMLoadFloat4(&_boundingBox.Orientation)); }

private:
	BoundingOrientedBox _boundingBox;
	Vector3 _axis[3];
};

