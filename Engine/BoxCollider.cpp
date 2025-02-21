#include "pch.h"
#include "BoxCollider.h"

BoxCollider::BoxCollider() : Super(ColliderType::Box)
{
	//_boundingBox.Center = GetTransform()->GetPosition();
}

BoxCollider::~BoxCollider()
{

}

bool BoxCollider::IsCollide(shared_ptr<Collider>& other)
{
	ColliderType otherType = other->GetColliderType();

	if (otherType == ColliderType::Box)
		return _boundingBox.Intersects(static_pointer_cast<BoxCollider>(other)->GetBoundingBox());
}
