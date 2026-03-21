#include "pch.h"
#include "Rigidbody.h"

REGISTER_COMPONENT(Rigidbody);

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
{
	_isGravity = true;

	_velocity = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat4(&_rotationOffsetQuaternion, XMQuaternionIdentity());
}

Rigidbody::~Rigidbody()
{
	cout << "Released - Rigidbody:" << _id << "\n";
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
		_isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
		_isStatic ? Layers::NON_MOVING : Layers::MOVING);

	bodySettings.mIsSensor = _isTrigger;
	bodySettings.mUserData = reinterpret_cast<JPH::uint64>(_gameObject.lock().get());
	bodySettings.mGravityFactor = _isGravity ? 1.0f : 0.0f;

	// 시스템 등록
	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	_bodyID = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);

	if (!_isPhysicsActive)
		bodyInterface.RemoveBody(_bodyID);

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
	if (!_isPhysicsActive) return;

	if (_gameObject.lock()->GetFramesDirty() > 0) {
		auto transform = _gameObject.lock()->GetTransform();
		auto pos = transform->GetPosition();
		auto rot = transform->GetQuaternion();

		XMVECTOR finalQuat = XMQuaternionMultiply(XMLoadFloat4(&_rotationOffsetQuaternion), XMLoadFloat4(&rot));

		pos = pos + XMVector3Rotate(XMLoadFloat3(&_colliderOffset), finalQuat);

		PHYSICS->GetPhysicsSystem()->GetBodyInterface().SetPositionAndRotation(
			_bodyID,
			JPH::RVec3(pos.x, pos.y, pos.z),
			JPH::Quat(finalQuat),
			JPH::EActivation::Activate
		);
	}
}

void Rigidbody::OnDestroy()
{
	cout << "OnDestroy - Rigidbody:" << _id << "\n";

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.RemoveBody(_bodyID);
	bodyInterface.DestroyBody(_bodyID);
	PHYSICS->DeleteRigidbody(static_pointer_cast<Rigidbody>(shared_from_this()));
}

void Rigidbody::LoadXML(XMLElement* compElem)
{
	SetStatic(compElem->BoolAttribute("Static", false));
	SetGravity(compElem->BoolAttribute("Gravity", true));
	SetColliderTrigger(compElem->BoolAttribute("Trigger", false));
	SetPhysicsActive(compElem->BoolAttribute("PhysicsActive", true));

	XMLElement* colliderElem = compElem->FirstChildElement("Collider");
	if (colliderElem) {
		const char* colliderShape = colliderElem->Attribute("Shape");
		if (colliderShape != 0) {
			string shapeStr(colliderShape);
			if (shapeStr == "Box") SetColliderShape(ColliderShape::Box);
			else if (shapeStr == "Sphere") SetColliderShape(ColliderShape::Sphere);
			else if (shapeStr == "Capsule") SetColliderShape(ColliderShape::Capsule);
		}

		XMLElement* extentElem = colliderElem->FirstChildElement("Extent");
		if (extentElem) {
			SetColliderExtents({ extentElem->FloatAttribute("x", 0.5f), extentElem->FloatAttribute("y", 0.5f), extentElem->FloatAttribute("z", 0.5f) });
		}
		SetColliderHalfHeight(colliderElem->FloatAttribute("Height", 0.5f));
		SetColliderRadius(colliderElem->FloatAttribute("Radius", 0.5f));

		XMLElement* offsetElem = colliderElem->FirstChildElement("Offset");
		if (offsetElem) {
			SetColliderOffset({ offsetElem->FloatAttribute("x", 0.0f), offsetElem->FloatAttribute("y", 0.0f), offsetElem->FloatAttribute("z", 0.0f) });
		}

		XMLElement* rotOffsetElem = colliderElem->FirstChildElement("RotOffset");
		if (rotOffsetElem) {
			SetColliderRotationOffset({ rotOffsetElem->FloatAttribute("x", 0.0f), rotOffsetElem->FloatAttribute("y", 0.0f), rotOffsetElem->FloatAttribute("z", 0.0f) });
		}
	}
}

void Rigidbody::SaveXML(XMLElement* compElem)
{
	compElem->SetAttribute("ComponentType", "Rigidbody");

	compElem->SetAttribute("Static", _isStatic);
	compElem->SetAttribute("Gravity", _isStatic);
	compElem->SetAttribute("Trigger", _isStatic);
	compElem->SetAttribute("PhysicsActive", _isStatic);

	XMLElement* colliderElem = compElem->InsertNewChildElement("Collider");
	colliderElem->SetAttribute("Shape", magic_enum::enum_name(_colliderShape).data());
	colliderElem->SetAttribute("Height", _height);
	colliderElem->SetAttribute("Radius", _radius);

	XMLElement* extentElem = colliderElem->InsertNewChildElement("Extent");
	extentElem->SetAttribute("x", _extents.x);
	extentElem->SetAttribute("y", _extents.y);
	extentElem->SetAttribute("z", _extents.z);

	XMLElement* offsetElem = colliderElem->InsertNewChildElement("Offset");
	offsetElem->SetAttribute("x", _colliderOffset.x);
	offsetElem->SetAttribute("y", _colliderOffset.y);
	offsetElem->SetAttribute("z", _colliderOffset.z);

	XMLElement* rotOffsetElem = colliderElem->InsertNewChildElement("RotOffset");
	rotOffsetElem->SetAttribute("x", _colliderRotationOffset.x);
	rotOffsetElem->SetAttribute("y", _colliderRotationOffset.y);
	rotOffsetElem->SetAttribute("z", _colliderRotationOffset.z);
}

void Rigidbody::SetColliderExtents(const Vector3& extents)
{
	_extents = extents;

	if (_colliderShape == ColliderShape::Box)
		UpdateShapeData();
}

void Rigidbody::SetColliderTrigger(bool value)
{
	if (value == _isTrigger) return;
	_isTrigger = value;

	if (!isInitialized) return;

	PHYSICS->GetPhysicsSystem()->GetBodyInterface().SetIsSensor(_bodyID, _isTrigger);
}

void Rigidbody::SetColliderHalfHeight(float height)
{
	_height = height;

	if (_colliderShape == ColliderShape::Capsule)
		UpdateShapeData();
}

void Rigidbody::SetColliderRadius(float radius)
{
	_radius = radius;

	if (_colliderShape == ColliderShape::Sphere)
		UpdateShapeData();
}

void Rigidbody::SetColliderOffset(const Vector3& offset)
{
	_colliderOffset = offset;
}

void Rigidbody::SetColliderRotationOffset(const Vector3& angle)
{
	_colliderRotationOffset = angle;
	Vector3 radian = MathHelper::DegreeToRadian(_colliderRotationOffset);
	XMStoreFloat4(
		&_rotationOffsetQuaternion, 
		XMQuaternionRotationRollPitchYaw(radian.x, radian.y, radian.z)
	);
}

void Rigidbody::SetColliderShape(ColliderShape colliderShape)
{
	if (_colliderShape == colliderShape) return;

	_colliderShape = colliderShape;

	CreateShape();

	UpdateShapeData();
}

void Rigidbody::SetGravity(bool value)
{
	if (value == _isGravity) return;
	_isGravity = value;

	if (!isInitialized) return;

	PHYSICS->GetPhysicsSystem()->GetBodyInterface().SetGravityFactor(_bodyID, _isGravity ? 1.0f : 0.0f);
}

void Rigidbody::SetStatic(bool value)
{
	if (value == _isStatic) return;
	_isStatic = value;

	if (!isInitialized) return;

	PHYSICS->GetPhysicsSystem()->GetBodyInterface().SetMotionType(
		_bodyID,
		_isStatic ? EMotionType::Static : EMotionType::Dynamic,
		EActivation::Activate);
}

void Rigidbody::SetPhysicsActive(bool value)
{
	if (value == _isPhysicsActive) return;
	_isPhysicsActive = value;

	if (!isInitialized) return;

	if (_isPhysicsActive)
		PHYSICS->GetPhysicsSystem()->GetBodyInterface().AddBody(_bodyID, EActivation::Activate);
	else
		PHYSICS->GetPhysicsSystem()->GetBodyInterface().RemoveBody(_bodyID);
}

void Rigidbody::CreateShape()
{
	if (!isInitialized) {
		_shapeDataDirtyFlag = true;
		return;
	}

	if (_colliderShape == ColliderShape::Box) {
		JPH::BoxShapeSettings boxShapeSetting(JPH::Vec3(_extents.x, _extents.y, _extents.z));
		_shapeResult = boxShapeSetting.Create();
	}
	else if (_colliderShape == ColliderShape::Sphere) {
		JPH::SphereShapeSettings sphereShapeSetting(_radius);
		_shapeResult = sphereShapeSetting.Create();
	}
	else if (_colliderShape == ColliderShape::Capsule) {
		JPH::CapsuleShapeSettings capsuleShapeSetting;
		capsuleShapeSetting.mHalfHeightOfCylinder = _height;
		capsuleShapeSetting.mRadius = _radius;
		_shapeResult = capsuleShapeSetting.Create();
	}
}

void Rigidbody::UpdateShapeData()
{
	if (!isInitialized) {
		_shapeDataDirtyFlag = true;
		return;
	}

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.SetShape(
		_bodyID, 
		_shapeResult.Get(), 
		true, 
		_isPhysicsActive ? JPH::EActivation::Activate : JPH::EActivation::DontActivate
	);
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

	_extents = Vector3((maxX - minX) / 2, (maxY - minY) / 2, (maxZ - minZ) / 2);

	JPH::BoxShapeSettings shapeSettings(JPH::Vec3(_extents.x, _extents.y, _extents.z));
	return shapeSettings.Create();
}
