#pragma once
#include "Component.h"
class Rigidbody : public Component
{
	using Super = Component;
public:
	Rigidbody();
	virtual ~Rigidbody();

	void FixedUpdate()override;
	void Update()override;

public:
	void AddForce(Vector3 force);

public:
	bool isGravity;
	static const float gravitationalAcceleration;

private:
	Vector3 _velocity;
	Vector3 _acceleration;
	float _elasticModulus;
};

