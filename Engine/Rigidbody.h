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
	Vector3 _velocity;			// �ӵ�
	Vector3 _netForce;			// �շ�
	float _elasticModulus;		// ź�����
	float _friction;			// �������
	float _mass;				// ����
	float _drag;				// �������� ���
};

