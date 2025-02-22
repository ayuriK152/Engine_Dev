#include "pch.h"
#include "Rigidbody.h"

const float Rigidbody::gravitationalAcceleration = -9.8f;

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
{
	isGravity = true;
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
	_elasticModulus = 0.5f;
}

Rigidbody::~Rigidbody()
{

}

void Rigidbody::FixedUpdate()
{
	if (isGravity)
		_acceleration.y = gravitationalAcceleration;
	auto collider = GetGameObject()->GetComponent<Collider>();
	if (collider != nullptr)
	{
		if (collider->IsOnColliding())
		{
			//_velocity = MathHelper::VectorReverseSign(MathHelper::VectorMultiply(_velocity, _elasticModulus));
			_velocity = MathHelper::VectorReflect(MathHelper::VectorMultiply(_velocity, _elasticModulus), collider->GetCollidingVector());
			_acceleration.y = 0;
		}
	}
}

void Rigidbody::Update()
{
	_velocity = MathHelper::VectorAddition(_velocity, MathHelper::VectorMultiply(_acceleration, TIME->DeltaTime()));
	GetTransform()->Translate(MathHelper::VectorMultiply(_velocity, TIME->DeltaTime()));
}

void Rigidbody::AddForce(Vector3 force)
{
	_velocity = MathHelper::VectorAddition(_velocity, force);
}

