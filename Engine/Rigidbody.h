#pragma once
#include "Component.h"

#define GRAVITY -9.81f

class Rigidbody : public Component
{
	using Super = Component;
public:
	Rigidbody();
	virtual ~Rigidbody();

	void Init()override;
	void FixedUpdate()override;
	void Update()override;

public:
	void SetColliderSize(const Vector3& size);

	Vector3 GetVelocity() { return _velocity; }
	void SetVelocity(Vector3& veclocity) { _velocity = veclocity; }

	BodyID GetBodyID() { return _bodyID; }

private:
	JPH::ShapeSettings::ShapeResult FitOnMesh();

public:
	bool isGravity;				// 중력 여부

private:
	Vector3 _velocity;			// 속도

	JPH::BodyID _bodyID;
};

