#include "pch.h"
#include "CharacterController.h"

CharacterController::CharacterController() : Component(ComponentType::CharacterController)
{

}

CharacterController::~CharacterController()
{

}

void CharacterController::Init()
{
	JPH::CapsuleShapeSettings shapeSettings;
	shapeSettings.mHalfHeightOfCylinder = _height;
	shapeSettings.mRadius = _radius;
	_shape = shapeSettings.Create().Get();

	JPH::CharacterVirtualSettings characterSettings;
	characterSettings.mShape = _shape;
	characterSettings.mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
	characterSettings.mMaxStrength = 100.0f;
	characterSettings.mCharacterPadding = 0.02f;
	characterSettings.mMass = 80.0f;
	characterSettings.mMaxStrength = 1000.0f;

	Vector3 pos = GetTransform()->GetPosition();

	_character = new JPH::CharacterVirtual(
		&characterSettings,
		JPH::RVec3(pos.x, pos.y, pos.z),
		JPH::Quat::sIdentity(),
		PHYSICS->GetPhysicsSystem()
	);

	_character->SetShapeOffset(JPH::Vec3Arg(_offset.x, _offset.y, _offset.z));
}

void CharacterController::PreUpdate()
{
	// 중력 값을 상수로 정의하고 따로 받아오도록 리팩토링 필요함
	_verticalVelocity -= (9.8f * TIME->DeltaTime());
	_currentVelocity = _desiredVelocity;
	_currentVelocity.y += _verticalVelocity;
	JPH::Vec3 totalVelocity(_currentVelocity.x, _currentVelocity.y, _currentVelocity.z);
	_character->SetLinearVelocity(totalVelocity);
	_character->Update(
		TIME->DeltaTime(),
		-_character->GetUp() * 9.8f,
		PHYSICS->GetPhysicsSystem()->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
		PHYSICS->GetPhysicsSystem()->GetDefaultLayerFilter(Layers::MOVING),
		{ }, { }, *PHYSICS->GetTempAllocator()
	);

	JPH::Vec3 newPos = _character->GetPosition();
	GetTransform()->SetPosition(Vector3(newPos.GetX(), newPos.GetY(), newPos.GetZ()));

	if (_character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround)
		_verticalVelocity = 0.0f;


	_desiredVelocity = { 0.0f, 0.0f, 0.0f };
}
