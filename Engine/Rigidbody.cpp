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

}

void Rigidbody::Update()
{

}

void Rigidbody::SetColliderSize(const Vector3& size)
{
	if (!isInitialized) return;

	JPH::BoxShapeSettings shapeSettings(JPH::Vec3(size.x, size.y, size.z));
	JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.SetShape(_bodyID, shapeResult.Get(), true, JPH::EActivation::Activate);
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
