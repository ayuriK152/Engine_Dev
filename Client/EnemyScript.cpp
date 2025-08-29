#include "pch.h"
#include "EnemyScript.h"

void EnemyScript::Init()
{
	gameObject = GetGameObject();
	gameObject->SetTag("Enemy");
}

void EnemyScript::Update()
{
	if (_health <= 0)
	{
		if (gameObject->GetComponent<Animator>()->GetCurrentAnimation()->GetName() != "death")
		{
			gameObject->GetComponent<Animator>()->SetCurrentAnimation("death");
			gameObject->GetComponent<Animator>()->SetLoop(false);
		}
	}
}

void EnemyScript::TakeDamage(int damage)
{
	_health -= damage;
	DEBUG->Log("Take" + to_string(damage) + "damage. Remain Health - " + to_string(_health));
}
