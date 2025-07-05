#include "pch.h"
#include "BoxCollider.h"



BoxCollider::BoxCollider() : Super(ColliderType::Box)
{

}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::Init()
{
	_boundingBox.Center = GetTransform()->GetPosition();
	_colliders.push_back(static_pointer_cast<Collider>(shared_from_this()));
}

void BoxCollider::FixedUpdate()
{

}

void BoxCollider::Update()
{
	_boundingBox.Center = GetTransform()->GetPosition();
	for (auto& collider : _colliders)
	{
		if (collider == static_pointer_cast<Collider>(shared_from_this()))
			continue;

		_isOnColliding = IsCollide(collider);
	}
}

bool BoxCollider::IsCollide(shared_ptr<Collider>& other)
{
	switch (other->GetColliderType())
	{
		case ColliderType::Box:
		{
			auto boxCollider = static_pointer_cast<BoxCollider>(other);
			return _boundingBox.Intersects(boxCollider->GetBoundingBox());
			break;
		}
	}
	return false;
}
