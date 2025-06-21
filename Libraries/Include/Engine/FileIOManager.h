#pragma once

class FileIOManager
{
	DECLARE_SINGLE(FileIOManager);
public:
	~FileIOManager() = default;

	void Init();

public:
	void XMLFromMaterial(shared_ptr<Material> material, const wstring& name);

	void LoadTextures();
	void LoadMaterials();
	
	template<typename T>
	HANDLE CreateFileHandle(string fileName);

	template<typename T>
	void WriteToFile(HANDLE fileHandle, const T& data);

	void WriteToFile(HANDLE fileHandle, void* data, UINT32 size);
	void WriteToFile(HANDLE fileHandle, const string& data);

	void ReadFileData(HANDLE fileHandle, void* out, UINT32 dataSize);
	void ReadFileData(HANDLE fileHandle, string& out);
};

template<typename T>
HANDLE FileIOManager::CreateFileHandle(string fileName) {
	string fullPath;

	if (is_same_v<T, Mesh>)
		fullPath = RESOURCE_PATH_MESH + fileName + BULB_EXT_MESH;
	else if (is_same_v<T, Animation>)
		fullPath = RESOURCE_PATH_ANIMATION + fileName + BULB_EXT_ANIMATION;
	else if (is_same_v<T, Bone>)
		fullPath = RESOURCE_PATH_BONE + fileName + BULB_EXT_BONE;
	else if (is_same_v<T, GameObject>)
		fullPath = RESOURCE_PATH_PREFAB + fileName + BULB_EXT_PREFAB;
	else
		fullPath = RESOURCE_PATH + fileName + BULB_EXT_UNKNOWN;

	if (PathFileExistsA(fullPath.c_str())) {
		return CreateFileA(
			fullPath.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}
	else {
		return CreateFileA(
			fullPath.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}
}

template<typename T>
void FileIOManager::WriteToFile(HANDLE fileHandle, const T& data)
{
	WriteFile(
		fileHandle,
		&data,
		sizeof(T),
		NULL,
		NULL
	);
}
