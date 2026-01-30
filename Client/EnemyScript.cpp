#include "pch.h"
#include "EnemyScript.h"

void EnemyScript::Init()
{
	_gameObject = GetGameObject();
	_gameObject->SetTag("Enemy");

	_animator = _gameObject->GetComponent<Animator>();
	_animator->SetLoop(true);

	auto rigidbody = make_shared<Rigidbody>();
	rigidbody->SetColliderExtents(Vector3(0.3f, 0.9f, 0.3f));
	rigidbody->SetColliderOffset(Vector3(0.0f, 0.9f, 0.0f));
	_gameObject->AddComponent(rigidbody);
}

void EnemyScript::Update()
{
	if (_currentState != EnemyMovementState::DEATH) {
		if (_health <= 0) {
			_currentState = EnemyMovementState::DEATH;
		}

		else {
			Vector3 targetVec = target->GetTransform()->GetPosition() - _gameObject->GetTransform()->GetPosition();
			_targetDistance = targetVec.Length();
			if (_targetDistance >= 2.0f) {
				_gameObject->GetTransform()->Translate(targetVec.Normalize() * TIME->DeltaTime());
				_currentState = EnemyMovementState::WALK;
			}
			else {
				_currentState = EnemyMovementState::IDLE;
			}
			_gameObject->GetTransform()->LookAtWithNoRoll(-targetVec + _gameObject->GetTransform()->GetPosition());
		}
	}

	if (_lastState != _currentState) {
		_lastState = _currentState;

		if (_currentState == EnemyMovementState::IDLE) {
			_animator->SetCurrentAnimation("idle");
		}
		else if (_currentState == EnemyMovementState::WALK) {
			_animator->SetCurrentAnimation("walk_forward");
		}
		else if (_currentState == EnemyMovementState::DEATH) {
			_animator->SetCurrentAnimation("death1");
			_animator->SetLoop(false);
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
