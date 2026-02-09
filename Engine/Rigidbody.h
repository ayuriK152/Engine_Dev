#pragma once
#include "Component.h"

#define GRAVITY -9.81f

enum class ColliderShape
{
	Box,
	Sphere,
	Capsule
};

class Rigidbody : public Component
{
	using Super = Component;
public:
	Rigidbody();
	virtual ~Rigidbody();

	void Init() override;
	void PreUpdate() override;
	void Update() override;

	void OnDestroy() override;

public:
	// Box Only
	void SetColliderExtents(const Vector3& size);

	void SetColliderTrigger(bool value);

	// Capsule Only
	void SetColliderHalfHeight(float height);

	// Sphere, Capsule Only
	void SetColliderRadius(float radius);

	void SetColliderOffset(const Vector3& offset);

	Vector3 GetColliderOffset() { return _colliderOffset; }

	void SetColliderShape(ColliderShape colliderShape);

	Vector3 GetVelocity() { return _velocity; }
	void SetVelocity(Vector3& veclocity) { _velocity = veclocity; }

	BodyID GetBodyID() { return _bodyID; }

private:
	void CreateShape();
	void UpdateShapeData();
	JPH::ShapeSettings::ShapeResult FitOnMesh();

public:
	bool isGravity;				// 중력 여부

private:
	Vector3 _velocity;			// 속도
	bool _isTrigger = false;

	Vector3 _extents = { 0.5f, 0.5f, 0.5f };
	float _height = 0.5f;
	float _radius = 0.5f;
	Vector3 _colliderOffset = { 0.0f, 0.0f, 0.0f };

	ColliderShape _colliderShape = ColliderShape::Box;

	bool _shapeDataDirtyFlag = false;
	bool _colliderOffsetDirtyFlag = false;

	JPH::BodyID _bodyID;
	JPH::ShapeSettings::ShapeResult _shapeResult;
};

