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
	// velocity essential
	bool isGravity;				// �߷� ����
	float elasticModulus;		// ź�����
	float friction;				// �������
	float mass;					// ����
	float drag;					// �������� ���

	// angular essential
	bool isAngular;
	float angularDrag;			// ȸ�� ���� ���
	float angularVelocityPower;	// ȸ�� � ������

	bool isPenetrationNormalFixed;	// �浹 �� ħ�� ������ �̲����� ������ ���� �ʵ�

private:
	Vector3 _velocity;			// �ӵ�
	Vector3 _netForce;			// �շ�

	Vector3 _angularVelocity;	// ���ӵ�
	Vector3 _netTorque;			// ����ũ
	Vector3 _inertiaTensor;		// �ڽ��� �밢�� ��
};

