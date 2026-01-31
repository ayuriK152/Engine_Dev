#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
	using Super = Collider;
public:
	BoxCollider();
	virtual ~BoxCollider();

	virtual void Init()override;
	virtual void PreUpdate()override;
	virtual void Update()override;

public:
	virtual CollisionInfo CheckCollide(shared_ptr<Collider>& other)override;
	virtual void FitOnMesh()override;

	float CheckOBB(XMVECTOR& axis, XMMATRIX& rotA, XMMATRIX& rotB, BoundingOrientedBox& target);
	XMVECTOR CheckSphere(XMVECTOR& axis, XMVECTOR& dist, float extent);

	XMVECTOR GetContactPoint(const BoundingOrientedBox& box, XMMATRIX& rotA, XMMATRIX& rotB);

	BoundingOrientedBox& GetBoundingBox() { return _boundingBox; }
	XMMATRIX& GetBoundingBoxRotationMatrix() { return XMMatrixRotationQuaternion(XMLoadFloat4(&_boundingBox.Orientation)); }

	Vector3 GetExtent() { return Vector3(_boundingBox.Extents.x, _boundingBox.Extents.y, _boundingBox.Extents.z); }
	void SetExtent(const Vector3& extent);

private:

	XMVECTOR GetClosestPoint(const XMVECTOR& point, const XMMATRIX& rotation, const BoundingOrientedBox& box);

	BoundingOrientedBox _boundingBox;
};

