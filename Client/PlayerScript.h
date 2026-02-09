#pragma once
#include "Script.h"
#include "BaseState.h"

enum class PlayerMovementState
{
	IDLE,
	WALK,
	RUN,
	SLASH,
	ROLL,
	STRAFE
	//STRAFE_FORWARD,
	//STRAFE_BACK,
	//STRAFE_RIGHT,
	//STRAFE_LEFT
};

class PlayerScript : public Script
{
#pragma region Player State Class
	class IdleState : public BaseState<PlayerScript> {
	public:
		void StateStart(PlayerScript* owner) override;
		void StateUpdate(PlayerScript* owner) override;
	};

	class WalkState : public BaseState<PlayerScript> {
	public:
		void StateStart(PlayerScript* owner) override;
		void StateUpdate(PlayerScript* owner) override;
	};

	class RunState : public BaseState<PlayerScript> {
	public:
		void StateStart(PlayerScript* owner) override;
		void StateUpdate(PlayerScript* owner) override;
	};

	class SlashState : public BaseState<PlayerScript> {
	public:
		void StateStart(PlayerScript* owner) override;
		void StateUpdate(PlayerScript* owner) override;
	};

	class RollState : public BaseState<PlayerScript> {
	public:
		void StateStart(PlayerScript* owner) override;
		void StateUpdate(PlayerScript* owner) override;
	};

	class StrafeState : public BaseState<PlayerScript> {
	public:
		void StateStart(PlayerScript* owner) override;
		void StateUpdate(PlayerScript* owner) override;
	};
#pragma endregion

public:
	void Init() override;
	void Update() override;

	void OnCollisionEnter(shared_ptr<GameObject> other) override;

public:
	void Roll();
	void Attack();
	void Move();
	void LockOn();

private:
	void SetState(PlayerMovementState state) {
		if (_playerMovementState == state) return;
		_playerMovementState = state;
		_isStateChanged = true;
	}

private:
	shared_ptr<GameObject> _gameObject;
	shared_ptr<Transform> _transform;
	shared_ptr<Animator> _animator;
	shared_ptr<CharacterController> _controller;
	float _speed = 1.55f;

	PlayerMovementState _playerMovementState = PlayerMovementState::IDLE;
	bool _isStateChanged = false;

	shared_ptr<GameObject> _lockOnTarget;
	bool _isLockOn = false;

	Vector3 _movingDirection;

	vector<BaseState<PlayerScript>*> _states;
};
