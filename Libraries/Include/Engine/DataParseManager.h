#pragma once

#define MATERIAL_PATH "Materials/"

class DataParseManager
{
	DECLARE_SINGLE(DataParseManager);
public:
	~DataParseManager() = default;

public:
	template <typename T>
	void SaveData(T data, const wstring& name);

	void XMLFromMaterial(shared_ptr<Material> material, const wstring& name);
};

template <typename T>
void DataParseManager::SaveData(T data, const wstring& name)
{

}

