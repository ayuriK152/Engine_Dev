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
	DEBUG->AddDebugRender(static_pointer_cast<Collider>(shared_from_this()));
}

void BoxCollider::FixedUpdate()
{

}

void BoxCollider::Update()
{
	if (GetGameObject()->GetFramesDirty() > 0)
	{
		_boundingBox.Center = GetTransform()->GetPosition();
		Vector4 orientation;
		XMStoreFloat4(&orientation, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&GetTransform()->GetRotationRadian())));
		_boundingBox.Orientation = orientation;
	}

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
