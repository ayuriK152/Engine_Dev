#include "pch.h"
#include "Rigidbody.h"

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
{
	isGravity = true;
	elasticModulus = 0.3f;
	friction = 0.2f;
	mass = 1.0f;
	drag = 0.0f;

	isAngular = false;	// 회전 운동 개선중. 기본값 false, 개선시까지 사용하지 않기.
	angularDrag = 0.3f;
	angularVelocityPower = 100.0f;

	isPenetrationNormalFixed = false;

	_velocity = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };

	_angularVelocity = { 0.0f, 0.0f, 0.0f };
	_netTorque = { 0.0f, 0.0f, 0.0f };
	_inertiaTensor = { 1.0f, 1.0f, 1.0f };
}

Rigidbody::~Rigidbody()
{

}

void Rigidbody::Init()
{
	auto transform = GetTransform();
	auto pos = transform->GetPosition();
	auto rot = transform->GetQuaternion();

	// 1. Shape 생성 (기존 Collider 정보를 활용)
	// 예: BoxCollider라고 가정
	JPH::ShapeSettings::ShapeResult shapeResult = FitOnMesh();

	// 2. Body 생성 설정
	JPH::BodyCreationSettings bodySettings(shapeResult.Get(),
		JPH::RVec3(pos.x, pos.y, pos.z),
		JPH::Quat(rot.x, rot.y, rot.z, rot.w),
		isGravity ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static,
		isGravity ? Layers::MOVING : Layers::NON_MOVING);

	// 3. 시스템에 등록
	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	_bodyID = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);

	PHYSICS->AddRigidbody(static_pointer_cast<Rigidbody>(shared_from_this()));
}

void Rigidbody::FixedUpdate()
{
	return;

	if (isGravity)
		_netForce.y = GRAVITY;
	else
		_netForce.y = 0.0f;
}

void Rigidbody::Update()
{
	return;

	// 선형 운동
	_velocity = _velocity + _netForce * TIME->DeltaTime();
	//_velocity = _velocity * (1.0f - drag * TIME->DeltaTime()); // 저항 적용
	GetTransform()->Translate(_velocity * TIME->DeltaTime());

	// 회전 운동
	if (isAngular)
	{
		Vector3 angularAcceleration(
			_netTorque.x / _inertiaTensor.x * angularVelocityPower,
			_netTorque.y / _inertiaTensor.y * angularVelocityPower,
			_netTorque.z / _inertiaTensor.z * angularVelocityPower
		);
		_angularVelocity = _angularVelocity + angularAcceleration * TIME->DeltaTime();
		_angularVelocity = _angularVelocity * (1.0f - angularDrag * TIME->DeltaTime()); // 회전 저항 적용

		// 오일러 통합
		GetTransform()->Rotate(_angularVelocity * TIME->DeltaTime());
	}

	_netTorque = { 0.0f, 0.0f, 0.0f }; // 매 프레임 토크 초기화
}

void Rigidbody::SetColliderSize(const Vector3& size)
{
	JPH::BoxShapeSettings shapeSettings(JPH::Vec3(size.x, size.y, size.z));
	JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.SetShape(_bodyID, shapeResult.Get(), true, JPH::EActivation::Activate);
}

void Rigidbody::AddForce(const Vector3& force)
{
	_velocity = _velocity + force;
}

void Rigidbody::AddTorque(const Vector3& torque)
{
	_netTorque = _netTorque + torque;
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
