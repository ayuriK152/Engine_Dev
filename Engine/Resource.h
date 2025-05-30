#pragma once

enum class ResourceType
{
	Undefined,
	Mesh,
	Texture,
	Shader,
	Material,
	Animation,

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

	void SetName(wstring name) { _name = name; }
	wstring GetName() { return _name; }

	void SetPath(wstring path) { _path = path; }
	wstring GetPath() { return _path; }

protected:
	virtual void Load(const wstring& path) { }

protected:
	ResourceType _type;
	wstring _name;
	wstring _path;
	UINT32 _id = 0;
};
