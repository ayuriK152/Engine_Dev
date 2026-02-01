#pragma once
#include "Resource.h"

class Mesh;

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager);
public:
	~ResourceManager() = default;

public:
	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> resource);

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	template<typename T>
	const map<wstring, shared_ptr<Resource>> GetByType();

	template<typename T>
	ResourceType GetResourceType();

	bool CheckResourceExists(const string& filePath);

	void CreateDefaultResources();

	void SaveMesh(shared_ptr<Mesh> mesh, const string& filePath = "");
	void SaveAnimation(shared_ptr<Animation> animation, const string& filePath = "");
	void SaveBone(map<string, BoneData> bones, const string& boneName, const string& filePath = "");
	void SavePrefab(shared_ptr<GameObject> prefabObject, const string& filePath = "");
	void SavePrefabRecursive(HANDLE fileHandle, shared_ptr<GameObject> object, int parentIdx, const string& prefabName);

	shared_ptr<Mesh> LoadMesh(const string& filePath);
	shared_ptr<Animation> LoadAnimation(const string& filePath);
	map<string, BoneData> LoadBone(const string& filePath);
	vector<shared_ptr<GameObject>> LoadPrefabObject(const string& filePath);

	ComponentType MapLegacyComponentType(UINT32 legacyType);

private:
	using KeyObjMap = map<wstring, shared_ptr<Resource>>;
	array<KeyObjMap, RESOURCE_TYPE_COUNT> _resources;
	set<string> _resourcePaths;
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

	_resourcePaths.insert(static_pointer_cast<Resource>(resource)->GetPath());
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
const map<wstring, shared_ptr<Resource>>
ResourceManager::GetByType()
{
	ResourceType type = GetResourceType<T>();
	return _resources[static_cast<UINT8>(type)];
	/*
	if (type != ResourceType::Undefined)
		return _resources[static_cast<UINT8>(type)];

	return nullptr;
	*/
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