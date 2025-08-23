#include "pch.h"
#include "EnemyScript.h"

void EnemyScript::Init()
{
	gameObject = GetGameObject();
	gameObject->SetTag("Enemy");
}

void EnemyScript::Update()
{

}
