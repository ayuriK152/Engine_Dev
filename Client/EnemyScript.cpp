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

	_patterns.push_back(new IdleState(static_pointer_cast<Script>(shared_from_this())));
	_patterns.push_back(new TrackWalkState(static_pointer_cast<Script>(shared_from_this())));
	_patterns.push_back(new DeathState(static_pointer_cast<Script>(shared_from_this())));
}

void EnemyScript::Update()
{
	if (_currentState != EnemyMovementState::DEATH) {
		if (_health <= 0) {
			_currentState = EnemyMovementState::DEATH;
			_isStateChanged = true;
		}

		else {
			_targetVec = target->GetTransform()->GetPosition() - _gameObject->GetTransform()->GetPosition();
			_targetDistance = _targetVec.Length();
			if (_targetDistance >= 2.0f && _currentState != EnemyMovementState::WALK) {
				_currentState = EnemyMovementState::WALK;
				_isStateChanged = true;
			}
			else if (_targetDistance < 2.0f && _currentState != EnemyMovementState::IDLE) {
				_currentState = EnemyMovementState::IDLE;
				_isStateChanged = true;
			}
		}
	}

	if (_isStateChanged) {
		_patterns[static_cast<int>(_currentState)]->StateStart();
		_isStateChanged = false;
	}

	_patterns[static_cast<int>(_currentState)]->StateUpdate();
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

IdleState::IdleState(shared_ptr<Script> script)
{
	_script = static_pointer_cast<EnemyScript>(script);
}

void IdleState::StateStart()
{
	_script->_animator->SetCurrentAnimation("idle");
}

void IdleState::StateUpdate()
{
	_script->_gameObject->GetTransform()->LookAtWithNoRoll(-_script->_targetVec + _script->_gameObject->GetTransform()->GetPosition());
}

TrackWalkState::TrackWalkState(shared_ptr<Script> script)
{
	_script = static_pointer_cast<EnemyScript>(script);
}

void TrackWalkState::StateStart()
{
	_script->_animator->SetCurrentAnimation("walk_forward");
}

void TrackWalkState::StateUpdate()
{
	_script->_gameObject->GetTransform()->LookAtWithNoRoll(-_script->_targetVec + _script->_gameObject->GetTransform()->GetPosition());
	_script->controller->SetVelocity(_script->_targetVec.Normalize() * 1.1f);
}

DeathState::DeathState(shared_ptr<Script> script)
{
	_script = static_pointer_cast<EnemyScript>(script);
}

void DeathState::StateStart()
{
	_script->_animator->SetCurrentAnimation("death1");
	_script->_animator->SetLoop(false);
}
