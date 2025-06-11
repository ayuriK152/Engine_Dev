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

	template<typename T>
	void ReadFileData(HANDLE fileHandle, T& out);

	void ReadFileData(HANDLE fileHandle, string& out);
};

template<typename T>
HANDLE FileIOManager::CreateFileHandle(string fileName) {
	string fullPath;

	if (is_same_v<T, Mesh>)
		fullPath = RESOURCE_PATH_MESH + fileName + BULB_EXT_MESH;
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
	if (is_same_v<T, string>)
	{
	}
	WriteFile(
		fileHandle,
		&data,
		sizeof(T),
		NULL,
		NULL
	);
}

template<typename T>
void FileIOManager::ReadFileData(HANDLE fileHandle, T& out)
{
	void* data;
	ReadFile(
		fileHandle,
		&data,
		sizeof(T),
		NULL,
		NULL
	);

	out = (T)data;
}

