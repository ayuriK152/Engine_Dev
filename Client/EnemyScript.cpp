#include "pch.h"
#include "EnemyScript.h"

void EnemyScript::Init()
{
	gameObject = GetGameObject();
	gameObject->SetTag("Enemy");

	auto rigidbody = make_shared<Rigidbody>();
	rigidbody->SetColliderExtents(Vector3(0.3f, 0.9f, 0.3f));
	rigidbody->SetColliderOffset(Vector3(0.0f, 0.9f, 0.0f));
	gameObject->AddComponent(rigidbody);
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

void EnemyScript::OnCollisionEnter(shared_ptr<GameObject> other)
{
	if (other->GetTag() == "AttackAlly") {
		DEBUG->Log("Attacked");
		TakeDamage(10);
		// other->Delete(0.0f);
	}
}

void EnemyScript::TakeDamage(int damage)
{
	_health -= damage;
	DEBUG->Log("Take" + to_string(damage) + "damage. Remain Health - " + to_string(_health));
}
