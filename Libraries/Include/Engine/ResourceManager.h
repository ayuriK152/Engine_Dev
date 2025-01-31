#pragma once
#include "Resource.h"

class Mesh;

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager);
	~ResourceManager();
public:
	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> resource);

	template<typename T>
	ResourceType GetResourceType();

private:
	using KeyObjMap = map<wstring, Resource>;
	array<KeyObjMap, RESOURCE_TYPE_COUNT> _resources;
};

template<typename T>
bool ResourceManager::Add(const wstring& key, shared_ptr<T> resource)
{
	ResourceType type = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<UINT8>(type)];

	// 중복 추가 방지
	auto found = keyObjMap.find(key);
	if (found != keyObjMap.end())
		return false;

	keyObjMap[key] = resource;
	return true;
}

template<typename T>
ResourceType ResourceManager::GetResourceType()
{
	if (is_same<T, Mesh>)
		return ResourceType::Mesh;
	if (is_same<T, Texture>)
		return ResourceType::Texture;
	if (is_same<T, Material>)
		return ResourceType::Material;

	return ResourceType::Undefined;
}