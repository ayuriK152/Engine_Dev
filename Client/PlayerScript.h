#pragma once
#include "Script.h"
class PlayerScript : public Script
{
public:
	void Init() override;
	void Update() override;

public:
	bool Move();

private:
	shared_ptr<GameObject> gameObject;
	shared_ptr<Transform> transform;
	shared_ptr<Animator> animator;
	float speed = 1.75f;
};

