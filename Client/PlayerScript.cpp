#include "pch.h"
#include "PlayerScript.h"

void PlayerScript::Init()
{
	gameObject = GetGameObject();
	transform = gameObject->GetTransform();
	animator = gameObject->GetComponent<Animator>();

	_playerMovementState = IDLE;
	_lastMovementState = IDLE;
	_movingDirection = { 0.0f, 0.0f, 0.0f };
}

void PlayerScript::Update()
{
	Move();

	if (_playerMovementState != _lastMovementState)
	{
		_lastMovementState = _playerMovementState;

		switch (_playerMovementState)
		{
		case IDLE:
			animator->SetCurrentAnimation("idle_sword_4");
			break;

		case WALK:
			animator->SetCurrentAnimation("walk_sword_1");
			break;
		case RUN:
			animator->SetCurrentAnimation("run_sword_1");
			break;
		}
	}

	switch (_playerMovementState)
	{
	case WALK:
		transform->Translate(_movingDirection * TIME->DeltaTime() * speed);
		transform->LookAtWithNoRoll(transform->GetPosition() - _movingDirection);
		break;

	case RUN:
		transform->Translate(_movingDirection * TIME->DeltaTime() * speed * 3.0f);
		transform->LookAtWithNoRoll(transform->GetPosition() - _movingDirection);
		break;
	}
}

void PlayerScript::Move()
{
	_movingDirection = { 0.0f, 0.0f, 0.0f };

	if (INPUTM->IsKeyPress(KeyValue::W))
	{
		_movingDirection.z += 1;
	}
	if (INPUTM->IsKeyPress(KeyValue::S))
	{
		_movingDirection.z -= 1;
	}
	if (INPUTM->IsKeyPress(KeyValue::A))
	{
		_movingDirection.x -= 1;;
	}
	if (INPUTM->IsKeyPress(KeyValue::D))
	{
		_movingDirection.x += 1;
	}

	if (abs(_movingDirection.x) < 1.0f && abs(_movingDirection.z) < 1.0f)
	{
		_playerMovementState = IDLE;
		return;
	}

	_movingDirection = _movingDirection.Normalize();
	_playerMovementState = INPUTM->IsKeyPress(KeyValue::SHIFT) ? RUN : WALK;
}
