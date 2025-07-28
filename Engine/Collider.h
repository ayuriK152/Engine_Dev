#pragma once
#include "Component.h"

enum ColliderType
{
	Box,
	Sphere
};

struct CollisionInfo
{
	bool IsCollide;
	Vector3 ContactPoint;
	Vector3 Normal;
	float Depth;

	CollisionInfo() {
		IsCollide = false;
		Normal = { 0.0f, 0.0f, 0.0f };
		Depth = FLT_MAX;
	}
};

class Collider : public Component
{
	using Super = Component;
public:
	Collider(ColliderType type);
	virtual ~Collider();

public:
	virtual CollisionInfo CheckCollide(shared_ptr<Collider>& other) = 0;

	ColliderType GetColliderType() { return _colliderType; }
	Vector3 GetCollidingDirection() { return _collidingDir; }
	bool IsOnColliding() { return _isOnColliding; }

protected:
	ColliderType _colliderType;

	Vector3 _collidingDir;
	bool _isOnColliding = false;

	friend class PhysicsManager;
};

