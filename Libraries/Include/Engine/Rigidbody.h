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
	void SetColliderOffset(const Vector3& offset);
	Vector3 GetColliderOffset() { return _colliderOffset; }

	Vector3 GetVelocity() { return _velocity; }
	void SetVelocity(Vector3& veclocity) { _velocity = veclocity; }

	BodyID GetBodyID() { return _bodyID; }

private:
	void UpdateColliderSize();
	void UpdateColliderOffset();
	JPH::ShapeSettings::ShapeResult FitOnMesh();

public:
	bool isGravity;				// 중력 여부

private:
	Vector3 _velocity;			// 속도
	Vector3 _colliderSize = { 1.0f, 1.0f, 1.0f };
	Vector3 _colliderOffset = { 0.0f, 0.0f, 0.0f };

	bool _colliderSizeDirtyFlag = false;
	bool _colliderOffsetDirtyFlag = false;

	JPH::BodyID _bodyID;
	JPH::ShapeSettings::ShapeResult _shapeResult;
};

