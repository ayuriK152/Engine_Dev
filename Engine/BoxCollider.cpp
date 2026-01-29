#include "pch.h"
#include "BoxCollider.h"



BoxCollider::BoxCollider() : Super(ColliderType::Box)
{
	_boundingBox.Extents = { 0.5f, 0.5f, 0.5f };
}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::Init()
{
	_boundingBox.Center = GetTransform()->GetPosition();
	PHYSICS->AddCollider(static_pointer_cast<Collider>(shared_from_this()));
	FitOnMesh();
	Update();
}

void BoxCollider::FixedUpdate()
{

}

void BoxCollider::Update()
{
	if (GetGameObject()->GetFramesDirty() > 0)
	{
		XMVECTOR rotation = XMLoadFloat4(&GetTransform()->GetQuaternion());
		_boundingBox.Center = GetTransform()->GetPosition() + XMVector3Rotate(XMLoadFloat3(&_offset), rotation);
		XMStoreFloat4(&_boundingBox.Orientation, rotation);
	}
}

CollisionInfo BoxCollider::CheckCollide(shared_ptr<Collider>& other)
{
	CollisionInfo collInfo;

	switch (other->GetColliderType())
	{
		case ColliderType::Box:
		{
			collInfo.IsCollide = true;

			auto boxCollider = static_pointer_cast<BoxCollider>(other);
			XMMATRIX rotA = GetBoundingBoxRotationMatrix();
			XMMATRIX rotB = boxCollider->GetBoundingBoxRotationMatrix();

			// rotA 분리축 검사
			for (int i = 0; i < 3; i++)
			{
				if (XMVector3LengthSq(rotA.r[i]).m128_f32[0] < 1e-6)
					continue;
				float overlap = CheckOBB(rotA.r[i], rotA, rotB, boxCollider->GetBoundingBox());
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
				float overlap = CheckOBB(rotB.r[i], rotA, rotB, boxCollider->GetBoundingBox());
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
					float overlap = CheckOBB(crossVec, rotA, rotB, boxCollider->GetBoundingBox());
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

			XMVECTOR centerVec = XMLoadFloat3(&boxCollider->GetBoundingBox().Center) - XMLoadFloat3(&_boundingBox.Center);
			float dot = XMVector3Dot(centerVec, XMLoadFloat3(&collInfo.Normal)).m128_f32[0];
			if (dot < 0.0f)
				collInfo.Normal = collInfo.Normal * -1.0f;

			XMVECTOR contactPoint = GetContactPoint(boxCollider->GetBoundingBox(), rotA, rotB);
			collInfo.ContactPoint = Vector3(contactPoint.m128_f32[0], contactPoint.m128_f32[1], contactPoint.m128_f32[2]);

			break;
		}

		case ColliderType::Sphere:
		{
			auto sphereCollider = static_pointer_cast<SphereCollider>(other);
			BoundingSphere sphere = sphereCollider->GetBoundingSphere();
			XMMATRIX rot = GetBoundingBoxRotationMatrix();

			XMVECTOR d = XMLoadFloat3(&_boundingBox.Center) - XMLoadFloat3(&sphere.Center);
			auto extents = XMLoadFloat3(&_boundingBox.Extents).m128_f32;
			XMVECTOR closest = XMLoadFloat3(&_boundingBox.Center);

			for (int i = 0; i < 3; i++)
				closest += CheckSphere(rot.r[i], d, extents[i]);

			XMVECTOR diff = XMLoadFloat3(&sphere.Center) - closest;
			float diffLength = XMVector3Length(XMLoadFloat3(&sphere.Center) - closest).m128_f32[0];

			if (diffLength * diffLength < sphere.Radius * sphere.Radius)
			{
				collInfo.IsCollide = true;
				Vector3 normal;
				XMStoreFloat3(&normal, XMVector3Normalize(diff));
				collInfo.Normal = diffLength > 1e-6 ? normal : Vector3(1.0f, 0.0f, 0.0f);
				collInfo.Depth = sphere.Radius - diffLength;
			}

			break;
		}
	}
	return collInfo;
}

void BoxCollider::FitOnMesh()
{
	auto meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();
	if (meshRenderer == nullptr)
	{
		DEBUG->WarnLog("BoxCollider::FitOnMesh() - MeshRenderer not found.");
		return;
	}

	auto mesh = meshRenderer->GetMesh();
	if (mesh == nullptr)
	{
		DEBUG->WarnLog("BoxCollider::FitOnMesh() - Mesh not found.");
		return;
	}

	Vector3 scale = GetTransform()->GetScale();
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
	float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;
	for (auto& v : mesh->GetVertices())
	{
		Vector3 vertex = v.Position * scale;

		if (minX > vertex.x) minX = vertex.x;
		if (minY > vertex.y) minY = vertex.y;
		if (minZ > vertex.z) minZ = vertex.z;

		if (maxX < vertex.x) maxX = vertex.x;
		if (maxY < vertex.y) maxY = vertex.y;
		if (maxZ < vertex.z) maxZ = vertex.z;
	}

	_boundingBox.Extents = { (maxX - minX) * 0.5f, (maxY - minY) * 0.5f, (maxZ - minZ) * 0.5f };
	_offset = { (maxX + minX) * 0.5f, (maxY + minY) * 0.5f, (maxZ + minZ) * 0.5f };
}

float BoxCollider::CheckOBB(XMVECTOR& axis, XMMATRIX& rotA, XMMATRIX& rotB, BoundingOrientedBox& target)
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

XMVECTOR BoxCollider::CheckSphere(XMVECTOR& axis, XMVECTOR& dist, float extent)
{
	float centerDist = fabs(XMVector3Dot(dist, axis).m128_f32[0]);
	centerDist = clamp(centerDist, -extent, extent);
	return axis * centerDist;
}

XMVECTOR BoxCollider::GetContactPoint(const BoundingOrientedBox& box, XMMATRIX& rotA, XMMATRIX& rotB)
{
	XMVECTOR centerA = XMLoadFloat3(&_boundingBox.Center);
	XMVECTOR centerB = XMLoadFloat3(&box.Center);

	XMVECTOR pointOnA = GetClosestPoint(centerB, rotA, _boundingBox);
	XMVECTOR pointOnB = GetClosestPoint(centerA, rotB, box);

	XMVECTOR contact = 0.5f * (pointOnA + pointOnB);
	return contact;
}

void BoxCollider::SetExtent(const Vector3& extent)
{
	_boundingBox.Extents = { extent.x, extent.y, extent.z };
}

XMVECTOR BoxCollider::GetClosestPoint(const XMVECTOR& point, const XMMATRIX& rotation, const BoundingOrientedBox& box)
{
	XMVECTOR center = XMLoadFloat3(&box.Center);
	XMVECTOR dist = point - center;

	float x = XMVectorGetX(XMVector3Dot(dist, rotation.r[0]));
	float y = XMVectorGetX(XMVector3Dot(dist, rotation.r[1]));
	float z = XMVectorGetX(XMVector3Dot(dist, rotation.r[2]));

	x = max(-box.Extents.x, min(x, box.Extents.x));
	y = max(-box.Extents.y, min(y, box.Extents.y));
	z = max(-box.Extents.z, min(z, box.Extents.z));

	return center + rotation.r[0] * x + rotation.r[1] * y + rotation.r[2] * z;
}
