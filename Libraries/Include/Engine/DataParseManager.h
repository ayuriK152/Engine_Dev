#pragma once

#define MATERIAL_PATH "Materials/"

class DataParseManager
{
	DECLARE_SINGLE(DataParseManager);
public:
	~DataParseManager() = default;

	void Init();

public:
	template <typename T>
	void SaveData(T data, const wstring& name);

	void XMLFromMesh(shared_ptr<Mesh> mesh, const wstring& name);
	void XMLFromMaterial(shared_ptr<Material> material, const wstring& name);

	void LoadTextures();
	void LoadMaterials();
};

template <typename T>
void DataParseManager::SaveData(T data, const wstring& name)
{

}

