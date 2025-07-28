#include "pch.h"
#include "Rigidbody.h"

Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
{
	isGravity = true;
	isAngular = false;	// 회전 운동 개선중. 기본값 false, 개선시까지 사용하지 않기.

	_velocity = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };

	_angularVelocity = { 0.0f, 0.0f, 0.0f };
	_netTorque = { 0.0f, 0.0f, 0.0f };
	_inertiaTensor = { 1.0f, 1.0f, 1.0f };

	elasticModulus = 0.3f;
	friction = 0.2f;
	mass = 1.0f;
	drag = 0.0f;
	angularDrag = 0.3f;
	angularVelocityPower = 100.0f;
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
	// 선형 운동
	_velocity = MathHelper::VectorAddition(_velocity, MathHelper::VectorMultiply(_netForce, TIME->DeltaTime()));
	GetTransform()->Translate(MathHelper::VectorMultiply(_velocity, TIME->DeltaTime()));

	// 회전 운동
	if (isAngular)
	{
		Vector3 angularAcceleration(
			_netTorque.x / _inertiaTensor.x * angularVelocityPower,
			_netTorque.y / _inertiaTensor.y * angularVelocityPower,
			_netTorque.z / _inertiaTensor.z * angularVelocityPower
		);
		_angularVelocity = MathHelper::VectorAddition(_angularVelocity, MathHelper::VectorMultiply(angularAcceleration, TIME->DeltaTime()));
		_angularVelocity = MathHelper::VectorMultiply(_angularVelocity, 1.0f - angularDrag * TIME->DeltaTime()); // 회전 저항 적용

		// 오일러 통합
		GetTransform()->Rotate(MathHelper::VectorMultiply(_angularVelocity, TIME->DeltaTime()));
	}

	_netTorque = { 0.0f, 0.0f, 0.0f }; // 매 프레임 토크 초기화
}

void Rigidbody::AddForce(const Vector3& force)
{
	_velocity = MathHelper::VectorAddition(_velocity, force);
}

void Rigidbody::AddTorque(const Vector3& torque)
{
	_netTorque = MathHelper::VectorAddition(_netTorque, torque);
}

