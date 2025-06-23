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
		_namew = UniversalUtils::ToWString(name);
	}
	void SetName(const wstring& name) { 
		_namew = name;
	}
	string GetName() { return UniversalUtils::ToString(_namew); }
	wstring GetNameW() { return _namew; }

	void SetPath(const string& path) {
		_pathw = UniversalUtils::ToWString(path);
	}
	void SetPath(const wstring& path) { 
		_pathw = path; 
	}
	string GetPath() { return UniversalUtils::ToString(_pathw); }
	wstring GetPathW() { return _pathw; }

protected:
	virtual void Load(const wstring& path) { }

protected:
	ResourceType _type;

	wstring _namew;

	wstring _pathw;

	UINT32 _id = 0;
};
