#include "pch.h"
#include "EnemyScript.h"

void EnemyScript::Init()
{
	gameObject = GetGameObject();
	gameObject->SetTag("Enemy");

	animator = gameObject->GetComponent<Animator>();
	animator->SetLoop(true);

	auto rigidbody = make_shared<Rigidbody>();
	rigidbody->SetColliderExtents(Vector3(0.3f, 0.9f, 0.3f));
	rigidbody->SetColliderOffset(Vector3(0.0f, 0.9f, 0.0f));
	gameObject->AddComponent(rigidbody);
}

void EnemyScript::Update()
{
	if (_health <= 0 && _currentState != EnemyMovementState::DEATH)
	{
		_currentState = EnemyMovementState::DEATH;
	}

	if (_lastState != _currentState) {
		_lastState = _currentState;

		if (_currentState == EnemyMovementState::IDLE) {
			animator->SetCurrentAnimation("idle");
		}
		else if (_currentState == EnemyMovementState::DEATH) {
			animator->SetCurrentAnimation("death1");
			animator->SetLoop(false);
		}
	}
}

void EnemyScript::OnCollisionEnter(shared_ptr<GameObject> other)
{
	if (other->GetTag() == "AttackAlly") {
		if (_health > 0) {
			TakeDamage(10);
		}
	}
}

void EnemyScript::TakeDamage(int damage)
{
	_health -= damage;
	DEBUG->Log("Take" + to_string(damage) + "damage. Remain Health - " + to_string(_health));
}
