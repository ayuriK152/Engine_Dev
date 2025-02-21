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

protected:
	ColliderType _colliderType;
};

