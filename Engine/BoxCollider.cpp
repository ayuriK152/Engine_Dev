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
	auto mesh = GetGameObject()->GetComponent<MeshRenderer>()->GetMesh();
	BoundingOrientedBox::CreateFromPoints(
		_boundingBox, 
		mesh->GetVertexCount(),
		&mesh->GetVertices()[0].Position,
		sizeof(Vertex));
	_colliders.push_back(static_pointer_cast<BoxCollider>(shared_from_this()));
}

void BoxCollider::Update()
{
	
	_boundingBox.Center = GetTransform()->GetPosition();
	auto quaternion = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&GetTransform()->GetRotationRadian()));
	XMStoreFloat4(&_boundingBox.Orientation, quaternion);
	for (auto& c : _colliders)
	{
		if (c == shared_from_this())
			continue;
		
		if (IsCollide(c))
			cout << "Colliding" << endl;
		else
			cout << "Not Colliding" << endl;
	}
}

bool BoxCollider::IsCollide(shared_ptr<Collider>& other)
{
	ColliderType otherType = other->GetColliderType();

	if (otherType == ColliderType::Box)
		return _boundingBox.Intersects(static_pointer_cast<BoxCollider>(other)->GetBoundingBox());
}
