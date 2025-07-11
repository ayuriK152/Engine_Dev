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

	float GetElastic() { return _elasticModulus; }
	float GetMass() { return _mass; }

	void AddForce(Vector3 force);

public:
	bool isGravity;

private:
	Vector3 _velocity;			// 속도
	Vector3 _netForce;			// 합력
	float _elasticModulus;		// 탄성계수
	float _friction;			// 마찰계수
	float _mass;				// 질량
	float _drag;				// 공기저항 계수
};

