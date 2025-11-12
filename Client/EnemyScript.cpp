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
		//if (gameObject->GetComponent<Animator>()->GetCurrentAnimation()->GetName() != "death")
		//{
		//	gameObject->GetComponent<Animator>()->SetCurrentAnimation("death");
		//	gameObject->GetComponent<Animator>()->SetLoop(false);
		//}
	}
}

void EnemyScript::OnCollision(shared_ptr<Collider> other)
{
	if (other->GetGameObject()->GetTag() == "AttackAlly") {
		DEBUG->Log("Attacked");
		other->GetGameObject()->Delete(0.0f);
	}
}

void EnemyScript::TakeDamage(int damage)
{
	_health -= damage;
	DEBUG->Log("Take" + to_string(damage) + "damage. Remain Health - " + to_string(_health));
}
