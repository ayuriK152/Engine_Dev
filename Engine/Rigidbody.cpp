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

	// 1. Shape 생성
	_shapeResult = FitOnMesh();

	// 2. Body 생성 설정
	JPH::BodyCreationSettings bodySettings(_shapeResult.Get(),
		JPH::RVec3(pos.x, pos.y, pos.z),
		JPH::Quat(rot.x, rot.y, rot.z, rot.w),
		isGravity ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static,
		isGravity ? Layers::MOVING : Layers::NON_MOVING);

	// 3. 시스템에 등록
	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	_bodyID = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);

	if (_colliderSizeDirtyFlag) {
		_colliderSizeDirtyFlag = false;
		UpdateColliderSize();
	}

	PHYSICS->AddRigidbody(static_pointer_cast<Rigidbody>(shared_from_this()));
}

void Rigidbody::FixedUpdate()
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

void Rigidbody::SetColliderSize(const Vector3& size)
{
	_colliderSize = size;

	if (!isInitialized) {
		_colliderSizeDirtyFlag = true;
		return;
	}

	UpdateColliderSize();
}

void Rigidbody::SetColliderOffset(const Vector3& offset)
{
	_colliderOffset = offset;
}

void Rigidbody::UpdateColliderSize()
{
	JPH::BoxShapeSettings shapeSettings(JPH::Vec3(_colliderSize.x, _colliderSize.y, _colliderSize.z));
	_shapeResult = shapeSettings.Create();

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.SetShape(_bodyID, _shapeResult.Get(), true, JPH::EActivation::Activate);
}

void Rigidbody::UpdateColliderOffset()
{
	JPH::OffsetCenterOfMassShapeSettings offsetSettings(JPH::Vec3(_colliderOffset.x, _colliderOffset.y, _colliderOffset.z), _shapeResult.Get());
	_shapeResult = offsetSettings.Create();

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

	JPH::BoxShapeSettings shapeSettings(JPH::Vec3((maxX - minX) / 2, (maxY - minY) / 2, (maxZ - minZ) / 2));
	return shapeSettings.Create();
}
