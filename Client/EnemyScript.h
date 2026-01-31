#pragma once
#include "Script.h"

enum class EnemyMovementState
{
    IDLE,
    WALK,
    DEATH
};

class EnemyScript : public Script
{
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
    EnemyMovementState _lastState = EnemyMovementState::IDLE;

    float _targetDistance;
};

