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
	if (INPUTM->IsKeyDown(KeyValue::Q))
		LockOn();

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
	if (INPUTM->IsKeyDown(KeyValue::SPACE) && 
		!_animator->IsTransitionBlocked() && 
		_playerMovementState != PlayerMovementState::IDLE) {
		SetState(PlayerMovementState::ROLL);
	}
}

void PlayerScript::Attack()
{
	if (INPUTM->IsMouseLeftButtonDown() && 
		!_animator->IsTransitionBlocked() && 
		_playerMovementState != PlayerMovementState::SLASH) {
		SetState(PlayerMovementState::SLASH);
	}
}

void PlayerScript::Move()
{
	int moveZ = (INPUTM->IsKeyPress(KeyValue::W) ? 1 : 0) - (INPUTM->IsKeyPress(KeyValue::S) ? 1 : 0);
	int moveX = (INPUTM->IsKeyPress(KeyValue::D) ? 1 : 0) - (INPUTM->IsKeyPress(KeyValue::A) ? 1 : 0);

	if (moveZ == 0 && moveX == 0)
	{
		SetState(PlayerMovementState::IDLE);
		return;
	}

	shared_ptr<Transform> cameraTransform = Camera::GetCurrentCamera()->GetTransform();
	Vector3 look = cameraTransform->GetLook();
	Vector3 right = cameraTransform->GetRight();
	look.y = 0.0f;
	right.y = 0.0f;

	_movingDirection = (look * moveZ + right * moveX).Normalize();

	if (INPUTM->IsKeyPress(KeyValue::SHIFT))
		SetState(PlayerMovementState::RUN);
	else
		SetState(PlayerMovementState::WALK);
}

void PlayerScript::LockOn()
{
	if (_isLockOn) {
		_isLockOn = false;
		_lockOnTarget = nullptr;
		return;
	}


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
