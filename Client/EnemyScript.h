#pragma once
#include "Script.h"

enum class EnemyMovementState
{
    IDLE,
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

private:
    int _health = 100;
    EnemyMovementState _currentState = EnemyMovementState::IDLE;
    EnemyMovementState _lastState = EnemyMovementState::IDLE;
    shared_ptr<GameObject> gameObject;
    shared_ptr<Animator> animator;
};

