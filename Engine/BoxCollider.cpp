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
	auto meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();
	if (meshRenderer == nullptr) {

	}
	BoundingOrientedBox::CreateFromPoints(
		_boundingBox, 
		meshRenderer->GetMesh()->GetVertexCount(),
		&meshRenderer->GetMesh()->GetVertices()[0].Position,
		sizeof(Vertex));
	_colliders.push_back(static_pointer_cast<BoxCollider>(shared_from_this()));
}

void BoxCollider::FixedUpdate()
{
	_isOnColliding = false;
	_collidingVec = { 0.0f, 0.0f, 0.0f };

	_boundingBox.Center = GetTransform()->GetPosition();
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&GetTransform()->GetRotationRadian()));
	XMStoreFloat4(&_boundingBox.Orientation, quaternion);
	for (auto& c : _colliders)
	{
		if (c == shared_from_this())
			continue;

		if (IsCollide(c)) {
			cout << "Colliding" << endl;
		}
	}
	cout << _collidingVec.x << " " << _collidingVec.y << " " << _collidingVec.z << endl;
}

void BoxCollider::Update()
{
	
}

bool BoxCollider::IsCollide(shared_ptr<Collider>& other)
{
	ColliderType otherType = other->GetColliderType();

	if (otherType == ColliderType::Box) {
		auto boundary = static_pointer_cast<BoxCollider>(other)->GetBoundingBox();
		if (_boundingBox.Intersects(boundary)) {
			_collidingVec = MathHelper::VectorAddition(_collidingVec, MathHelper::VectorSubtract(_boundingBox.Center, boundary.Center));
			_isOnColliding = true;
			return true;
		}
		return false;
	}
}
