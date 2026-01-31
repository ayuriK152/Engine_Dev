#include "pch.h"
#include "Rigidbody.h"

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
{
	isGravity = true;

	_velocity = { 0.0f, 0.0f, 0.0f };
}

Rigidbody::~Rigidbody()
{

}

void Rigidbody::Init()
{
	auto transform = GetTransform();
	auto pos = transform->GetPosition();
	auto rot = transform->GetQuaternion();

	if (!_shapeDataDirtyFlag) {
		_shapeResult = FitOnMesh();
	}
	else {
		CreateShape();
	}

	// Body 생성 설정
	JPH::BodyCreationSettings bodySettings(_shapeResult.Get(),
		JPH::RVec3(pos.x, pos.y, pos.z),
		JPH::Quat(rot.x, rot.y, rot.z, rot.w),
		isGravity ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static,
		isGravity ? Layers::MOVING : Layers::NON_MOVING);

	bodySettings.mIsSensor = _isTrigger;
	bodySettings.mUserData = reinterpret_cast<JPH::uint64>(_gameObject.lock().get());

	// 시스템 등록
	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	_bodyID = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);

	if (_shapeDataDirtyFlag) {
		_shapeDataDirtyFlag = false;
		UpdateShapeData();
	}

	PHYSICS->AddRigidbody(static_pointer_cast<Rigidbody>(shared_from_this()));
}

void Rigidbody::PreUpdate()
{

}

void Rigidbody::Update()
{
	if (_gameObject.lock()->GetFramesDirty() > 0) {
		auto transform = _gameObject.lock()->GetTransform();
		auto pos = transform->GetPosition();
		auto rot = transform->GetQuaternion();

		pos = pos + XMVector3Rotate(XMLoadFloat3(&_colliderOffset), XMLoadFloat4(&rot));

		PHYSICS->GetPhysicsSystem()->GetBodyInterface().SetPositionAndRotation(
			_bodyID,
			JPH::RVec3(pos.x, pos.y, pos.z),
			JPH::Quat(rot.x, rot.y, rot.z, rot.w),
			JPH::EActivation::Activate
		);
	}
}

void Rigidbody::OnDestroy()
{
	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.RemoveBody(_bodyID);
	// bodyInterface.DestroyBody(_bodyID);
	PHYSICS->DeleteRigidbody(static_pointer_cast<Rigidbody>(shared_from_this()));
}

void Rigidbody::SetColliderExtents(const Vector3& extents)
{
	_colliderExtents = extents;

	if (_colliderShape == ColliderShape::Box)
		UpdateShapeData();
}

void Rigidbody::SetColliderTrigger(bool value)
{
	if (value == _isTrigger) return;

	_isTrigger = value;

	// Trigger는 런타임 중에 값이 바뀌면 body를 다시 만들어줘야함. 이부분 일단 구현 안해뒀으니까 나중에 해야함.
}

void Rigidbody::SetColliderRadius(float radius)
{
	_colliderRadius = radius;

	if (_colliderShape == ColliderShape::Shpere)
		UpdateShapeData();
}

void Rigidbody::SetColliderOffset(const Vector3& offset)
{
	_colliderOffset = offset;
}

void Rigidbody::SetColliderShape(ColliderShape colliderShape)
{
	if (_colliderShape == colliderShape) return;

	_colliderShape = colliderShape;

	CreateShape();

	UpdateShapeData();
}

void Rigidbody::CreateShape()
{
	if (!isInitialized) {
		_shapeDataDirtyFlag = true;
		return;
	}

	if (_colliderShape == ColliderShape::Box) {
		JPH::BoxShapeSettings boxShapeSetting(JPH::Vec3(_colliderExtents.x, _colliderExtents.y, _colliderExtents.z));
		_shapeResult = boxShapeSetting.Create();
	}
	else if (_colliderShape == ColliderShape::Shpere) {
		JPH::SphereShapeSettings sphereShapeSetting(_colliderRadius);
		_shapeResult = sphereShapeSetting.Create();
	}
}

void Rigidbody::UpdateShapeData()
{
	if (!isInitialized) {
		_shapeDataDirtyFlag = true;
		return;
	}

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.SetShape(_bodyID, _shapeResult.Get(), true, JPH::EActivation::Activate);
}

JPH::ShapeSettings::ShapeResult Rigidbody::FitOnMesh()
{
	auto meshRenderer = GetGameObject()->GetComponent<MeshRenderer>();
	if (meshRenderer == nullptr)
	{
		DEBUG->WarnLog("BoxCollider::FitOnMesh() - MeshRenderer not found.");
		JPH::BoxShapeSettings shapeSettings(JPH::Vec3(1.0f, 1.0f, 1.0f));
		return shapeSettings.Create();
	}

	auto mesh = meshRenderer->GetMesh();
	if (mesh == nullptr)
	{
		DEBUG->WarnLog("BoxCollider::FitOnMesh() - Mesh not found.");
		JPH::BoxShapeSettings shapeSettings(JPH::Vec3(1.0f, 1.0f, 1.0f));
		return shapeSettings.Create();
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

	_colliderExtents = Vector3((maxX - minX) / 2, (maxY - minY) / 2, (maxZ - minZ) / 2);

	JPH::BoxShapeSettings shapeSettings(JPH::Vec3(_colliderExtents.x, _colliderExtents.y, _colliderExtents.z));
	return shapeSettings.Create();
}
