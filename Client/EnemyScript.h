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
	friend class IdleState;
	friend class TrackWalkState;
	friend class DeathState;
public:
    void Init() override;
	void Update() override;

	void OnCollisionEnter(shared_ptr<GameObject> other) override;

public:
    void TakeDamage(int damage);

public:
	shared_ptr<GameObject> target;

private:
	shared_ptr<GameObject> _gameObject;
	shared_ptr<Animator> _animator;
	shared_ptr<CharacterController> controller;

    int _health = 100;
    EnemyMovementState _currentState = EnemyMovementState::IDLE;
    bool _isStateChanged = false;

    vector<BaseState*> _patterns;
    Vector3 _targetVec;

    float _targetDistance;
};

class IdleState : public BaseState {
public:
	IdleState(shared_ptr<Script> script);

	void StateStart() override;
	void StateUpdate() override;

private:
	shared_ptr<EnemyScript> _script;
};

class TrackWalkState : public BaseState {
public:
	TrackWalkState(shared_ptr<Script> script);

	void StateStart() override;
	void StateUpdate() override;

private:
	shared_ptr<EnemyScript> _script;
};

class DeathState : public BaseState {
public:
	DeathState(shared_ptr<Script> script);

	void StateStart() override;

private:
	shared_ptr<EnemyScript> _script;
};
