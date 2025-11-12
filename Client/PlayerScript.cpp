#include "pch.h"
#include "PlayerScript.h"
#include "EnemyScript.h"

void PlayerScript::Init()
{
	gameObject = GetGameObject();
	transform = gameObject->GetTransform();
	animator = gameObject->GetComponent<Animator>();
	animator->LoadAnimationEvents("..\\Resources\\Animations\\Paladin WProp J Nordstrom\\AnimationEvents.xml");
	animator->SetLoop(true);
	animator->SetCurrentAnimation("idle_sword_4");

	_playerMovementState = IDLE;
	_lastMovementState = IDLE;
	_movingDirection = { 0.0f, 0.0f, 0.0f };

	auto playerCollider = make_shared<BoxCollider>();
	playerCollider->SetOffset(Vector3(0.0f, 0.8f, 0.0f));
	playerCollider->SetExtent(Vector3(0.3f, 0.8f, 0.3f));
	gameObject->AddComponent(playerCollider);

	auto rigidbody = make_shared<Rigidbody>();
	rigidbody->elasticModulus = 0.0f;
	rigidbody->isPenetrationNormalFixed = true;
	gameObject->AddComponent(rigidbody);
}

void PlayerScript::Update()
{
	Roll();
	Attack();

	if (_playerMovementState == SLASH || _playerMovementState == ROLL)
	{
		if (animator->IsCurrentAnimationEnd())
		{
			animator->SetLoop(true);
			_playerMovementState = IDLE;
		}
	}
	else if (_playerMovementState != ROLL)
	{
		Move();
	}

	if (_playerMovementState != _lastMovementState)
	{
		switch (_playerMovementState)
		{
		case IDLE:
			if (_lastMovementState == SLASH)
			{
				animator->SetCurrentAnimation("idle_sword_4", 0.0f);
			}
			else
			{
				animator->SetCurrentAnimation("idle_sword_4");
			}
			break;

		case WALK:
			animator->SetCurrentAnimation("walk_sword_forward");
			break;
		case RUN:
			animator->SetCurrentAnimation("run_sword_forward");
			break;
		case SLASH:
			animator->SetCurrentAnimation("slash_1");
			break;
		case ROLL:
			animator->SetCurrentAnimation("roll");
			break;
		}

		_lastMovementState = _playerMovementState;
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
	case ROLL:
		transform->Translate(_movingDirection * TIME->DeltaTime() * speed * 3.0f);
		transform->LookAtWithNoRoll(transform->GetPosition() - _movingDirection);
		break;
	}
}

void PlayerScript::OnCollision(shared_ptr<Collider> other)
{
	if (other->GetGameObject()->GetTag() == "AttackHostile")
	{
		DEBUG->Log("Got Attack");
	}
}

void PlayerScript::Roll()
{
	if (INPUTM->IsKeyDown(KeyValue::SPACE) && !animator->IsTransitionBlocked())
	{
		animator->SetLoop(false);
		_playerMovementState = ROLL;
	}
}

void PlayerScript::Attack()
{

	if (INPUTM->IsMouseLeftButtonDown() && _playerMovementState != SLASH && !animator->IsTransitionBlocked())
	{
		animator->SetLoop(false);
		_playerMovementState = SLASH;
	}
}

void PlayerScript::Move()
{
	bool flags[4] = { false, false, false, false };
	_movingDirection = { 0.0f, 0.0f, 0.0f };

	if (INPUTM->IsKeyPress(KeyValue::W))
	{
		flags[0] = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::S))
	{
		if (flags[0])
			flags[0] = false;
		else
			flags[1] = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::A))
	{
		flags[2] = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::D)) 
	{
		if (flags[2])
			flags[2] = false;
		else
			flags[3] = true;
	}

	if (!flags[0] && !flags[1] && !flags[2] && !flags[3])
	{
		_playerMovementState = IDLE;
		return;
	}

	if (flags[0])
	{
		_movingDirection = _movingDirection + Camera::GetCurrentCamera()->GetTransform()->GetLook();
	}
	if (flags[1])
	{
		_movingDirection = _movingDirection - Camera::GetCurrentCamera()->GetTransform()->GetLook();
	}
	if (flags[2])
	{
		_movingDirection = _movingDirection - Camera::GetCurrentCamera()->GetTransform()->GetRight();
	}
	if (flags[3])
	{
		_movingDirection = _movingDirection + Camera::GetCurrentCamera()->GetTransform()->GetRight();
	}

	_movingDirection.y = 0.0f;
	_movingDirection = _movingDirection.Normalize();
	_playerMovementState = INPUTM->IsKeyPress(KeyValue::SHIFT) ? RUN : WALK;
}
