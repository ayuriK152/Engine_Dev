#include "pch.h"
#include "ComponentScriptTest.h"

shared_ptr<GameObject> gameObject;
shared_ptr<Transform> transform;

void ComponentScriptTest::Init()
{
	gameObject = GetGameObject();
	::transform = gameObject->GetTransform();
}

void ComponentScriptTest::FixedUpdate()
{

}

void ComponentScriptTest::Update()
{
	::transform->Translate(Vector3(0.0f, 0.0f, -1.5f * TIME->DeltaTime()));
}

void ComponentScriptTest::Render()
{

}
