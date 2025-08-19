#pragma once
#include "Script.h"

enum PlayerMovementState
{
	IDLE,
	WALK,
	RUN
};

class PlayerScript : public Script
{
public:
	void Init() override;
	void Update() override;

public:
	void Move();

private:
	shared_ptr<GameObject> gameObject;
	shared_ptr<Transform> transform;
	shared_ptr<Animator> animator;
	float speed = 1.5f;

	PlayerMovementState _playerMovementState;
	PlayerMovementState _lastMovementState;
	Vector3 _movingDirection;
	Vector3 _cameraForward;
	Vector3 _cameraRight;
};

