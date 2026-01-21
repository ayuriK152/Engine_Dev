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

	UINT GetID() { return _id; }

protected:
	virtual void Load(const wstring& path) { }

protected:
	ResourceType _type;

	wstring _namew;

	wstring _pathw;

	// 현재는 각 리소스 타입에 따라 따로 아이디를 관리하도록 하고있음.
	// 따라서 동일한 아이디를 갖는 다른 타입의 리소스가 존재할 수 있음.
	// 나중에 보고 문제가 될 것 같다 싶으면 로직 수정하기.
	// ex) id 1을 동시에 갖는 Mesh와 Animation -> 존재 가능
	UINT _id = 0;
};
