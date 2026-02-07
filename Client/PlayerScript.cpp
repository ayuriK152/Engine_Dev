#include "pch.h"
#include "PlayerScript.h"
#include "EnemyScript.h"

void PlayerScript::Init()
{
	_gameObject = GetGameObject();
	_transform = _gameObject->GetTransform();
	_animator = _gameObject->GetComponent<Animator>();
	_animator->LoadAnimationEvents("..\\Resources\\Animations\\Paladin WProp J Nordstrom\\AnimationEvents.xml");

	_playerMovementState = PlayerMovementState::IDLE;
	_isStateChanged = true;

	_movingDirection = { 0.0f, 0.0f, 0.0f };

	_controller = make_shared<CharacterController>();
	_controller->SetHalfHeight(0.5f);
	_controller->SetRadius(0.3f);
	_controller->SetOffset(Vector3(0.0f, 0.8f, 0.0f));
	_gameObject->AddComponent(_controller);

	_states.push_back(new IdleState());
	_states.push_back(new WalkState());
	_states.push_back(new RunState());
	_states.push_back(new SlashState());
	_states.push_back(new RollState());
}

void PlayerScript::Update()
{
	Roll();
	Attack();

	if (_playerMovementState == PlayerMovementState::SLASH || _playerMovementState == PlayerMovementState::ROLL)
	{
		if (_animator->IsCurrentAnimationEnd())
		{
			SetState(PlayerMovementState::IDLE);
		}
	}
	else if (_playerMovementState != PlayerMovementState::ROLL)
	{
		Move();
	}

	if (_isStateChanged)
	{
		_states[static_cast<int>(_playerMovementState)]->StateStart(this);
		_isStateChanged = false;
	}

	_states[static_cast<int>(_playerMovementState)]->StateUpdate(this);
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
	if (INPUTM->IsKeyDown(KeyValue::SPACE) && !_animator->IsTransitionBlocked() && _playerMovementState != PlayerMovementState::IDLE)
	{
		SetState(PlayerMovementState::ROLL);
	}
}

void PlayerScript::Attack()
{

	if (INPUTM->IsMouseLeftButtonDown() && _playerMovementState != PlayerMovementState::SLASH && !_animator->IsTransitionBlocked())
	{
		SetState(PlayerMovementState::SLASH);
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
		SetState(PlayerMovementState::IDLE);
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

	if (INPUTM->IsKeyPress(KeyValue::SHIFT))
		SetState(PlayerMovementState::RUN);
	else
		SetState(PlayerMovementState::WALK);
}

void PlayerScript::IdleState::StateStart(PlayerScript* owner)
{
	owner->_animator->SetCurrentAnimation("idle_sword_4");
	owner->_animator->SetLoop(true);
}

void PlayerScript::IdleState::StateUpdate(PlayerScript* owner)
{

}

void PlayerScript::WalkState::StateStart(PlayerScript* owner)
{
	owner->_animator->SetCurrentAnimation("walk_sword_forward");
	owner->_animator->SetLoop(true);
}

void PlayerScript::WalkState::StateUpdate(PlayerScript* owner)
{
	owner->_transform->LookAtWithNoRoll(owner->_transform->GetPosition() - owner->_movingDirection);
	owner->_controller->SetVelocity(owner->_movingDirection * owner->_speed);
}

void PlayerScript::RunState::StateStart(PlayerScript* owner)
{
	owner->_animator->SetCurrentAnimation("run_sword_forward");
	owner->_animator->SetLoop(true);
}

void PlayerScript::RunState::StateUpdate(PlayerScript* owner)
{
	owner->_transform->LookAtWithNoRoll(owner->_transform->GetPosition() - owner->_movingDirection);
	owner->_controller->SetVelocity(owner->_movingDirection * owner->_speed * 3.0f);
}

void PlayerScript::SlashState::StateStart(PlayerScript* owner)
{
	owner->_animator->SetCurrentAnimation("slash_1");
	owner->_animator->SetLoop(false);
}

void PlayerScript::SlashState::StateUpdate(PlayerScript* owner)
{

}

void PlayerScript::RollState::StateStart(PlayerScript* owner)
{
	owner->_animator->SetCurrentAnimation("roll");
 	owner->_animator->SetLoop(false);
}

void PlayerScript::RollState::StateUpdate(PlayerScript* owner)
{
	owner->_transform->LookAtWithNoRoll(owner->_transform->GetPosition() - owner->_movingDirection);
	owner->_controller->SetVelocity(owner->_movingDirection * owner->_speed * 3.0f);
}
