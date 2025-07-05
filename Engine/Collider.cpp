#include "pch.h"
#include "Collider.h"

vector<shared_ptr<Collider>> Collider::_colliders;

Collider::Collider(ColliderType type) : Super(ComponentType::Collider), _colliderType(type)
{

}

Collider::~Collider()
{

}
