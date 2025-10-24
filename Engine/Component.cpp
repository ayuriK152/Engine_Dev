#include "pch.h"
#include "Component.h"

UINT Component::_count = 0;

Component::Component(ComponentType type) : type(type)
{
	_id = _count++;
}

Component::~Component()
{

}

shared_ptr<GameObject> Component::GetGameObject()
{
	return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return _gameObject.lock()->GetTransform();
}
