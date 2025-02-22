#pragma once
#include "Component.h"

enum ColliderType
{
	Box,
	Sphere
};

class Collider : public Component
{
	using Super = Component;
public:
	Collider(ColliderType type);
	virtual ~Collider();

	virtual bool IsCollide(shared_ptr<Collider>& other) = 0;

	ColliderType GetColliderType() { return _colliderType; }
	Vector3 GetCollidingVector() { return _collidingVec; }
	bool IsOnColliding() { return _isOnColliding; }

protected:
	ColliderType _colliderType;
	static vector<shared_ptr<Collider>> _colliders;

	Vector3 _collidingVec;
	bool _isOnColliding;
};

