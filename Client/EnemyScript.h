#pragma once
#include "Script.h"
#include "BaseState.h"

enum class EnemyMovementState
{
    IDLE,
    WALK,
    DEATH
};

class EnemyScript : public Script
{
	class IdleState : public BaseState<EnemyScript> {
	public:
		void StateStart(EnemyScript* owner) override;
		void StateUpdate(EnemyScript* owner) override;
	};

	class TrackWalkState : public BaseState<EnemyScript> {
	public:
		void StateStart(EnemyScript* owner) override;
		void StateUpdate(EnemyScript* owner) override;
	};

	class DeathState : public BaseState<EnemyScript> {
	public:
		void StateStart(EnemyScript* owner) override;
	};

public:
	~EnemyScript();

    void Init() override;
	void Update() override;

	void OnCollisionEnter(shared_ptr<GameObject> other) override;

	void OnDestroy() override;

public:
    void TakeDamage(int damage);

private:
	void SetState(EnemyMovementState state) {
		if (_currentState == state) return;
		_currentState = state;
		_isStateChanged = true;
	}

	void SetDamageText(int damage);
	void UpdateDamageText();

public:
	shared_ptr<GameObject> target;

private:
	shared_ptr<GameObject> _gameObject;
	shared_ptr<Transform> _transform;
	shared_ptr<Animator> _animator;
	shared_ptr<CharacterController> _controller;
	shared_ptr<Rigidbody> _hitbox;

    int _health = 100;
    EnemyMovementState _currentState = EnemyMovementState::IDLE;
    bool _isStateChanged = false;

    vector<BaseState<EnemyScript>*> _patterns;
    Vector3 _targetVec;

	shared_ptr<UIText> _damageText;
	float _damageTextTime = 0.0f;
	int _cumulativeDamage = 0;

    float _targetDistance;
};
