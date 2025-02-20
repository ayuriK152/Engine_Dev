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

void Rigidbody::Update()
{
	if (isGravity)
		_velocity.y -= 9.8f * TIME->DeltaTime();

	GetTransform()->Translate(MathHelper::VectorMultiply(_velocity, TIME->DeltaTime()));
}

void Rigidbody::AddForce(Vector3 force)
{
	_velocity = MathHelper::VectorAddition(_velocity, force);
}
