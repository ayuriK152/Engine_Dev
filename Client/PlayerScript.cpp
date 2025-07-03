#include "pch.h"
#include "PlayerScript.h"

shared_ptr<GameObject> gameObject;
shared_ptr<Transform> transform;
float speed = 5.0f;

void PlayerScript::Init()
{
	gameObject = GetGameObject();
	::transform = gameObject->GetTransform();
}

void PlayerScript::Update()
{
	if (INPUTM->IsKeyPress(KeyValue::W))
		::transform->Translate(MathHelper::VectorMultiply(Vector3(0.0f, 0.0f, 1.0f), TIME->DeltaTime() * speed));
	if (INPUTM->IsKeyPress(KeyValue::S))
		::transform->Translate(MathHelper::VectorMultiply(Vector3(0.0f, 0.0f, 1.0f), -TIME->DeltaTime() * speed));
	if (INPUTM->IsKeyPress(KeyValue::A))
		::transform->Translate(MathHelper::VectorMultiply(Vector3(1.0f, 0.0f, 0.0f), -TIME->DeltaTime() * speed));
	if (INPUTM->IsKeyPress(KeyValue::D))
		::transform->Translate(MathHelper::VectorMultiply(Vector3(1.0f, 0.0f, 0.0f), TIME->DeltaTime() * speed));
}
