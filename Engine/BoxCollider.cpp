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
	_boundingBox.Extents = MathHelper::VectorMultiply(GetTransform()->GetScale(), 0.5f);
	DEBUG->AddDebugRender(static_pointer_cast<Collider>(shared_from_this()));
	PHYSICS->AddCollider(static_pointer_cast<Collider>(shared_from_this()));
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
}

CollisionInfo BoxCollider::CheckCollide(shared_ptr<Collider>& other)
{
	CollisionInfo collInfo;
	collInfo.Depth = FLT_MAX;

	switch (other->GetColliderType())
	{
		case ColliderType::Box:
		{
			auto boxCollider = static_pointer_cast<BoxCollider>(other);
			XMMATRIX rotA = GetBoundingBoxRotationMatrix();
			XMMATRIX rotB = boxCollider->GetBoundingBoxRotationMatrix();

			// rotA 분리축 검사
			for (int i = 0; i < 3; i++)
			{
				if (XMVector3LengthSq(rotA.r[i]).m128_f32[0] < 1e-6)
					continue;
				float overlap = CheckAxis(rotA.r[i], rotA, rotB, boxCollider->GetBoundingBox());
				if (overlap < 0.0f)
				{
					collInfo.IsCollide = false;
					return collInfo;
				}
				if (overlap < collInfo.Depth)
				{
					collInfo.Depth = overlap;
					XMStoreFloat3(&collInfo.Normal, rotA.r[i]);
				}
			}

			// rotB 분리축 검사
			for (int i = 0; i < 3; i++)
			{
				if (XMVector3LengthSq(rotB.r[i]).m128_f32[0] < 1e-6)
					continue;
				float overlap = CheckAxis(rotB.r[i], rotA, rotB, boxCollider->GetBoundingBox());
				if (overlap < 0.0f)
				{
					collInfo.IsCollide = false;
					return collInfo;
				}
				if (overlap < collInfo.Depth)
				{
					collInfo.Depth = overlap;
					XMStoreFloat3(&collInfo.Normal, rotB.r[i]);
				}
			}

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					XMVECTOR crossVec = XMVector3Cross(rotA.r[i], rotB.r[j]);
					if (XMVector3LengthSq(crossVec).m128_f32[0] < 1e-6)
						continue;
					float overlap = CheckAxis(crossVec, rotA, rotB, boxCollider->GetBoundingBox());
					if (overlap < 0.0f)
					{
						collInfo.IsCollide = false;
						return collInfo;
					}
					if (overlap < collInfo.Depth)
					{
						collInfo.Depth = overlap;
						XMStoreFloat3(&collInfo.Normal, crossVec);
					}
				}
			}

			break;
		}
	}
	return collInfo;
}

float BoxCollider::CheckAxis(XMVECTOR& axis, XMMATRIX& rotA, XMMATRIX& rotB, BoundingOrientedBox& target)
{
	XMVECTOR d = XMLoadFloat3(&_boundingBox.Center) - XMLoadFloat3(&target.Center);
	float centerDist = fabs(XMVector3Dot(d, axis).m128_f32[0]);

	float totalA = 0, totalB = 0;
	totalA += fabs(_boundingBox.Extents.x * XMVector3Dot(rotA.r[0], axis).m128_f32[0]);
	totalA += fabs(_boundingBox.Extents.y * XMVector3Dot(rotA.r[1], axis).m128_f32[0]);
	totalA += fabs(_boundingBox.Extents.z * XMVector3Dot(rotA.r[2], axis).m128_f32[0]);
	totalB += fabs(target.Extents.x * XMVector3Dot(rotB.r[0], axis).m128_f32[0]);
	totalB += fabs(target.Extents.y * XMVector3Dot(rotB.r[1], axis).m128_f32[0]);
	totalB += fabs(target.Extents.z * XMVector3Dot(rotB.r[2], axis).m128_f32[0]);

	return totalA + totalB - centerDist;
}
