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

	float CheckOBB(XMVECTOR& axis, XMMATRIX& rotA, XMMATRIX& rotB, BoundingOrientedBox& target);
	XMVECTOR CheckSphere(XMVECTOR& axis, XMVECTOR& dist, float extent);

	BoundingOrientedBox& GetBoundingBox() { return _boundingBox; }
	XMMATRIX& GetBoundingBoxRotationMatrix() { return XMMatrixRotationQuaternion(XMLoadFloat4(&_boundingBox.Orientation)); }

private:
	BoundingOrientedBox _boundingBox;
};

