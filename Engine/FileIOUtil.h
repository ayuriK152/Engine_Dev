#pragma once

class FileIOUtil
{
	DECLARE_SINGLE(FileIOUtil);
public:
	~FileIOUtil() = default;

	void Init();

public:
	void XMLFromMaterial(shared_ptr<Material> material, const wstring& name);

	void LoadTextures();
	void LoadMaterials();
	
	template<typename T>
	HANDLE CreateFileHandle(string fileName, bool createNotExists = true);

	HANDLE CreateFileHandle(string fileName, string ext = "");

	template<typename T>
	void WriteToFile(HANDLE fileHandle, const T& data);

	void WriteToFile(HANDLE fileHandle, void* data, UINT32 size);
	void WriteToFile(HANDLE fileHandle, const string& data);

	void ReadFileData(HANDLE fileHandle, void* out, UINT32 dataSize);
	void ReadFileData(HANDLE fileHandle, string& out);
};

template<typename T>
HANDLE FileIOUtil::CreateFileHandle(string fileName, bool createNotExists) {
	string fullPath;

	if (PathFileExistsA(fileName.c_str()))
		fullPath = fileName;
	else
	{
		if (is_same_v<T, Mesh>)
			fullPath = RESOURCE_PATH_MESH + fileName + BULB_EXT_MESH;
		else if (is_same_v<T, Animation>)
			fullPath = RESOURCE_PATH_ANIMATION + fileName + BULB_EXT_ANIMATION;
		else if (is_same_v<T, BoneData>)
			fullPath = RESOURCE_PATH_BONE + fileName + BULB_EXT_BONE;
		else if (is_same_v<T, GameObject>)
			fullPath = RESOURCE_PATH_PREFAB + fileName + BULB_EXT_PREFAB;
		else
			fullPath = RESOURCE_PATH + fileName + BULB_EXT_UNKNOWN;

		string subPath = fullPath.substr(0, fullPath.find_last_of('\\') + 1);
		if (!filesystem::exists(subPath))
			filesystem::create_directories(subPath);
	}

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
	else if (createNotExists){
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
	else {
		return INVALID_HANDLE_VALUE;
	}
}

template<typename T>
void FileIOUtil::WriteToFile(HANDLE fileHandle, const T& data) {
	WriteFile(
		fileHandle,
		&data,
		sizeof(T),
		NULL,
		NULL
	);
}
