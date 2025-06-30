#include "pch.h"
#include "ComponentScriptTest.h"

shared_ptr<GameObject> gameObject;
shared_ptr<Transform> transform;

void ComponentScriptTest::Init()
{
	gameObject = GetGameObject();
	::transform = gameObject->GetTransform();
}

void ComponentScriptTest::Update()
{

}
