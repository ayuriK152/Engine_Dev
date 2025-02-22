#include "pch.h"
#include "Collider.h"

Collider::Collider(ColliderType type) : Super(ComponentType::Collider), _colliderType(type)
{
	
}

Collider::~Collider()
{

}

vector<shared_ptr<Collider>> Collider::_colliders;
