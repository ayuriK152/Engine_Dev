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

	void AddForce(Vector3 force);

public:
	bool isGravity;				// �߷� ����
	float elasticModulus;		// ź�����
	float friction;				// �������
	float mass;					// ����
	float drag;					// �������� ���

private:
	Vector3 _velocity;			// �ӵ�
	Vector3 _netForce;			// �շ�
};

