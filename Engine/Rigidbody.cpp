#include "pch.h"
#include "Rigidbody.h"

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
{
	isGravity = true;

	_velocity = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };
	elasticModulus = 0.5f;
	friction = 0.2f;
	mass = 1.0f;
	drag = 0.0f;
}

Rigidbody::~Rigidbody()
{

}

void Rigidbody::FixedUpdate()
{
	if (isGravity)
		_netForce.y = GRAVITY;
	else
		_netForce.y = 0.0f;
}

void Rigidbody::Update()
{
	_velocity = MathHelper::VectorAddition(_velocity, MathHelper::VectorMultiply(_netForce, TIME->DeltaTime()));
	GetTransform()->Translate(MathHelper::VectorMultiply(_velocity, TIME->DeltaTime()));
}

void Rigidbody::AddForce(Vector3 force)
{
	_velocity = MathHelper::VectorAddition(_velocity, force);
}

