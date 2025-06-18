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

	void SetName(const string& name) { 
		_name = name;
		_namew = UniversalUtils::ToWString(name);
	}
	string GetName() { return _name; }

	void SetName(wstring name) { 
		_namew = name;
		_name = UniversalUtils::ToString(name);
	}
	wstring GetNameW() { return _namew; }

	void SetPath(wstring path) { _path = path; }
	wstring GetPath() { return _path; }

protected:
	virtual void Load(const wstring& path) { }

protected:
	ResourceType _type;

	string _name;
	wstring _namew;

	wstring _path;

	UINT32 _id = 0;
};
