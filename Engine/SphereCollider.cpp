#include "pch.h"
#include "SphereCollider.h"

SphereCollider::SphereCollider() : Super(ColliderType::Sphere)
{

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::Init()
{
	// 구 형태 어떻게 렌더링할거임?
	// DEBUG->AddDebugRender(static_pointer_cast<Collider>(shared_from_this()));
	PHYSICS->AddCollider(static_pointer_cast<Collider>(shared_from_this()));
	_boundingSphere.Radius = 0.5f;
}

void SphereCollider::FixedUpdate()
{

}

void SphereCollider::Update()
{
	if (GetGameObject()->GetFramesDirty() > 0)
	{
		_boundingSphere.Center = GetTransform()->GetPosition();
	}
}

CollisionInfo SphereCollider::CheckCollide(shared_ptr<Collider>& other)
{
	CollisionInfo collInfo;

	switch (other->GetColliderType())
	{
		case ColliderType::Box:
		{
			shared_ptr<BoxCollider> boxCollider = static_pointer_cast<BoxCollider>(other);
			BoundingOrientedBox box = boxCollider->GetBoundingBox();
			XMMATRIX rot = boxCollider->GetBoundingBoxRotationMatrix();

			XMVECTOR d = XMLoadFloat3(&_boundingSphere.Center) - XMLoadFloat3(&box.Center);
			auto extents = XMLoadFloat3(&box.Extents).m128_f32;
			XMVECTOR closest = XMLoadFloat3(&box.Center);

			for (int i = 0; i < 3; i++)
				closest += CheckSphere(rot.r[i], d, extents[i]);

			XMVECTOR diff = XMLoadFloat3(&_boundingSphere.Center) - closest;
			float diffLength = XMVector3Length(diff).m128_f32[0];

			if (diffLength * diffLength < _boundingSphere.Radius * _boundingSphere.Radius)
			{
				collInfo.IsCollide = true;
				Vector3 normal;
				XMStoreFloat3(&normal, -XMVector3Normalize(diff));
				collInfo.Normal = diffLength > 1e-6 ? normal : Vector3(1.0f, 0.0f, 0.0f);
				collInfo.Depth = _boundingSphere.Radius - diffLength;
				cout << _boundingSphere.Radius << " " << diffLength << " " << collInfo.Depth << " " << diffLength + collInfo.Depth << endl;
			}

			break;
		}
		case ColliderType::Sphere:
		{
			auto sphereCollider = static_pointer_cast<SphereCollider>(other);

			float dist = MathHelper::VectorLength(MathHelper::VectorSubtract(_boundingSphere.Center, sphereCollider->GetBoundingSphere().Center));
			float overlap = _boundingSphere.Radius + sphereCollider->GetBoundingSphere().Radius - dist;

			if (overlap < 0.0f)
				break;

			collInfo.IsCollide = true;
			collInfo.Normal = MathHelper::VectorNormalize(MathHelper::VectorSubtract(sphereCollider->GetBoundingSphere().Center, _boundingSphere.Center));
			collInfo.Depth = overlap;

			break;
		}
	}

	return collInfo;
}

XMVECTOR SphereCollider::CheckSphere(XMVECTOR& axis, XMVECTOR& dist, float extent)
{
	float centerDist = fabs(XMVector3Dot(dist, axis).m128_f32[0]);
	centerDist = clamp(centerDist, -extent, extent);
	return axis * centerDist;
}
