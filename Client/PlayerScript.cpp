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

	_movingDirection = { 0.0f, 0.0f, 0.0f };

	//auto rigidbody = make_shared<Rigidbody>();
	//rigidbody->SetColliderExtents(Vector3(0.3f, 0.8f, 0.3f));
	//rigidbody->SetColliderOffset(Vector3(0.0f, 0.8f, 0.0f));
	//gameObject->AddComponent(rigidbody);

	controller = make_shared<CharacterController>();
	controller->SetHalfHeight(0.5f);
	controller->SetRadius(0.3f);
	controller->SetOffset(Vector3(0.0f, 0.8f, 0.0f));
	gameObject->AddComponent(controller);
}

void PlayerScript::Update()
{
	Roll();
	Attack();

	if (_playerMovementState == PlayerMovementState::SLASH || _playerMovementState == PlayerMovementState::ROLL)
	{
		if (animator->IsCurrentAnimationEnd())
		{
			animator->SetLoop(true);
			_playerMovementState = PlayerMovementState::IDLE;
		}
	}
	else if (_playerMovementState != PlayerMovementState::ROLL)
	{
		Move();
	}

	if (_playerMovementState != _lastMovementState)
	{
		switch (_playerMovementState)
		{
		case PlayerMovementState::IDLE:
			if (_lastMovementState == PlayerMovementState::SLASH)
			{
				animator->SetCurrentAnimation("idle_sword_4", 0.0f);
			}
			else
			{
				animator->SetCurrentAnimation("idle_sword_4");
			}
			break;

		case PlayerMovementState::WALK:
			animator->SetCurrentAnimation("walk_sword_forward");
			break;
		case PlayerMovementState::RUN:
			animator->SetCurrentAnimation("run_sword_forward");
			break;
		case PlayerMovementState::SLASH:
			animator->SetCurrentAnimation("slash_1");
			break;
		case PlayerMovementState::ROLL:
			animator->SetCurrentAnimation("roll");
			break;
		}

		_lastMovementState = _playerMovementState;
	}

	switch (_playerMovementState)
	{
	case PlayerMovementState::WALK:
		controller->SetVelocity(_movingDirection * speed);
		// transform->Translate(_movingDirection * TIME->DeltaTime() * speed);
		transform->LookAtWithNoRoll(transform->GetPosition() - _movingDirection);
		break;

	case PlayerMovementState::RUN:
		controller->SetVelocity(_movingDirection * speed * 3.0f);
		// transform->Translate(_movingDirection * TIME->DeltaTime() * speed * 3.0f);
		transform->LookAtWithNoRoll(transform->GetPosition() - _movingDirection);
		break;
	case PlayerMovementState::ROLL:
		controller->SetVelocity(_movingDirection * speed * 3.0f);
		// transform->Translate(_movingDirection * TIME->DeltaTime() * speed * 3.0f);
		transform->LookAtWithNoRoll(transform->GetPosition() - _movingDirection);
		break;
	}
}

void PlayerScript::OnCollisionEnter(shared_ptr<GameObject> other)
{
	if (other->GetTag() == "AttackHostile")
	{
		DEBUG->Log("Got Attack");
	}
}

void PlayerScript::Roll()
{
	if (INPUTM->IsKeyDown(KeyValue::SPACE) && !animator->IsTransitionBlocked() && _playerMovementState != PlayerMovementState::IDLE)
	{
		animator->SetLoop(false);
		_playerMovementState = PlayerMovementState::ROLL;
	}
}

void PlayerScript::Attack()
{

	if (INPUTM->IsMouseLeftButtonDown() && _playerMovementState != PlayerMovementState::SLASH && !animator->IsTransitionBlocked())
	{
		animator->SetLoop(false);
		_playerMovementState = PlayerMovementState::SLASH;
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
		_playerMovementState = PlayerMovementState::IDLE;
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
	_playerMovementState = INPUTM->IsKeyPress(KeyValue::SHIFT) ? PlayerMovementState::RUN : PlayerMovementState::WALK;
}
