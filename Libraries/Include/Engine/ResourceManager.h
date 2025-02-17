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
	shared_ptr<T> Get(const wstring& key);

	template<typename T>
	ResourceType GetResourceType();

	void CreateDefaultResources();

private:
	using KeyObjMap = map<wstring, shared_ptr<Resource>>;
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
shared_ptr<T>
ResourceManager::Get(const wstring& key)
{
	ResourceType type = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<UINT8>(type)];

	auto found = keyObjMap.find(key);
	if (found != keyObjMap.end())
		return static_pointer_cast<T>(keyObjMap[key]);

	return nullptr;
}

template<typename T>
ResourceType ResourceManager::GetResourceType()
{
	if (is_same_v<T, Mesh>)
		return ResourceType::Mesh;
	if (is_same_v<T, Texture>)
		return ResourceType::Texture;
	if (is_same_v<T, Material>)
		return ResourceType::Material;
	if (is_same_v<T, Shader>)
		return ResourceType::Shader;

	return ResourceType::Undefined;
}