#include "pch.h"
#include "Component.h"

Component::Component(ComponentType type) : type(type)
{

}

Component::~Component()
{

}

shared_ptr<GameObject> Component::GetGameObject()
{
	return _gameObject.lock();
}
