#pragma once
#include "Script.h"

class EnemyScript :
    public Script
{
public:
    void Init() override;
	void Update() override;

public:
    void TakeDamage(int damage);

private:
    int _health = 100;
    shared_ptr<GameObject> gameObject;
};

