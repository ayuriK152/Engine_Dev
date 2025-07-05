#include "pch.h"
#include "SphereScript.h"

void SphereScript::Init()
{
	gameObject = GetGameObject();
	transform = gameObject->GetTransform();
}

void SphereScript::Update()
{
	//transform->Rotate(Vector3(0.0f, 60.0f * TIME->DeltaTime(), 0.0f));
}
