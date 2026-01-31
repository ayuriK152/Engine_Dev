#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
	using Super = Collider;
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void Init()override;
	virtual void PreUpdate()override;
	virtual void Update()override;

public:
	virtual CollisionInfo CheckCollide(shared_ptr<Collider>& other)override;
	XMVECTOR CheckSphere(XMVECTOR& axis, XMVECTOR& dist, float extent);

	BoundingSphere& GetBoundingSphere() { return _boundingSphere; }

private:
	BoundingSphere _boundingSphere;
};

