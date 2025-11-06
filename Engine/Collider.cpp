#include "pch.h"
#include "Collider.h"

Collider::Collider(ColliderType type) : Super(ComponentType::Collider), _colliderType(type)
{

}

Collider::~Collider()
{
	try
	{
		PHYSICS->DeleteCollider(static_pointer_cast<Collider>(shared_from_this()));
		DEBUG->DeleteDebugCollider(static_pointer_cast<Collider>(shared_from_this()));
	} catch (exception e)
	{

	}
}
