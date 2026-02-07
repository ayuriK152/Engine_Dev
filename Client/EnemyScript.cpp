#include "pch.h"
#include "EnemyScript.h"

void EnemyScript::Init()
{
	_gameObject = GetGameObject();
	_gameObject->SetTag("Enemy");

	_animator = _gameObject->GetComponent<Animator>();
	_animator->SetLoop(true);

	controller = make_shared<CharacterController>();
	controller->SetHalfHeight(0.6f);
	controller->SetRadius(0.3f);
	controller->SetOffset(Vector3(0.0f, 0.9f, 0.0f));
	_gameObject->AddComponent(controller);

	_patterns.push_back(new IdleState());
	_patterns.push_back(new TrackWalkState());
	_patterns.push_back(new DeathState());
}

void EnemyScript::Update()
{
	if (_currentState != EnemyMovementState::DEATH) {
		if (_health <= 0) {
			SetState(EnemyMovementState::DEATH);
		}

		else {
			_targetVec = target->GetTransform()->GetPosition() - _gameObject->GetTransform()->GetPosition();
			_targetDistance = _targetVec.Length();
			if (_targetDistance >= 2.0f && _currentState != EnemyMovementState::WALK) {
				SetState(EnemyMovementState::WALK);
			}
			else if (_targetDistance < 2.0f && _currentState != EnemyMovementState::IDLE) {
				SetState(EnemyMovementState::IDLE);
			}
		}
	}

	if (_isStateChanged) {
		_patterns[static_cast<int>(_currentState)]->StateStart(this);
		_isStateChanged = false;
	}

	_patterns[static_cast<int>(_currentState)]->StateUpdate(this);
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

void EnemyScript::IdleState::StateStart(EnemyScript* owner)
{
	owner->_animator->SetCurrentAnimation("idle");
}

void EnemyScript::IdleState::StateUpdate(EnemyScript* owner)
{
	owner->_gameObject->GetTransform()->LookAtWithNoRoll(-owner->_targetVec + owner->_gameObject->GetTransform()->GetPosition());
}

void EnemyScript::TrackWalkState::StateStart(EnemyScript* owner)
{
	owner->_animator->SetCurrentAnimation("walk_forward");
}

void EnemyScript::TrackWalkState::StateUpdate(EnemyScript* owner)
{
	owner->_gameObject->GetTransform()->LookAtWithNoRoll(-owner->_targetVec + owner->_gameObject->GetTransform()->GetPosition());
	owner->controller->SetVelocity(owner->_targetVec.Normalize() * 1.1f);
}

void EnemyScript::DeathState::StateStart(EnemyScript* owner)
{
	owner->_animator->SetCurrentAnimation("death1");
	owner->_animator->SetLoop(false);
}
