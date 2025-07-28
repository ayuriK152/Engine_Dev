#pragma once
#include "Component.h"

#define GRAVITY -9.81f

class Rigidbody : public Component
{
	using Super = Component;
public:
	Rigidbody();
	virtual ~Rigidbody();

	void FixedUpdate()override;
	void Update()override;

public:
	Vector3 GetVelocity() { return _velocity; }
	void SetVelocity(Vector3& veclocity) { _velocity = veclocity; }

	void AddForce(const Vector3& force);
	void AddTorque(const Vector3& torque);

public:
	bool isGravity;				// 중력 여부
	float elasticModulus;		// 탄성계수
	float friction;				// 마찰계수
	float mass;					// 질량
	float drag;					// 공기저항 계수

	bool isAngular;
	float angularDrag;			// 회전 저항 계수
	float angularVelocityPower;	// 회전 운동 에너지

private:
	Vector3 _velocity;			// 속도
	Vector3 _netForce;			// 합력

	Vector3 _angularVelocity;	// 각속도
	Vector3 _netTorque;			// 합토크
	Vector3 _inertiaTensor;		// 박스면 대각선 값
};

