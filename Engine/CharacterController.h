#pragma once
#include "Component.h"

class CharacterController : public Component
{
public:
	CharacterController();
	~CharacterController() override;

	void Init() override;
	void PreUpdate() override;

public:
	void SetHalfHeight(float height) { _height = height; }
	void SetRadius(float radius) { _radius = radius; }
	void SetOffset(Vector3 offset) { _offset = offset; }

	void SetVelocity(Vector3 velocity) { _desiredVelocity = velocity; }

private:
	JPH::Ref<JPH::CharacterVirtual> _character;
	JPH::Ref<JPH::Shape> _shape;
	JPH::BodyID _bodyId;

	float _height = 0.5f;
	float _radius = 0.5f;
	Vector3 _offset = { 0.0f, 0.0f, 0.0f };

	Vector3 _currentVelocity = { 0.0f, 0.0f, 0.0f };
	Vector3 _desiredVelocity = { 0.0f, 0.0f, 0.0f };
	float _verticalVelocity = 0.0f;
};

