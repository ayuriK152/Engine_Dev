#pragma once
#include "Component.h"
class Rigidbody : public Component
{
	using Super = Component;
public:
	Rigidbody();
	virtual ~Rigidbody();

	void Update() override;

public:
	void AddForce(Vector3 force);

public:
	bool isGravity;

private:
	Vector3 _velocity;
};

