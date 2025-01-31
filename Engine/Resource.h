#pragma once

enum class ResourceType
{
	Undefined,
	Mesh,
	Texture,
	Material,

	End
};

enum
{
	RESOURCE_TYPE_COUNT = static_cast<UINT8>(ResourceType::End)
};

class Resource
{
public:
	Resource(ResourceType type);
	virtual ~Resource();

	ResourceType GetType() { return _type; }
protected:
	ResourceType _type;
	wstring _name;
	wstring _path;
	UINT32 _id = 0;
};

