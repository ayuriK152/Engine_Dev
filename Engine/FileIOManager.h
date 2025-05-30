#pragma once

#define MATERIAL_PATH "Materials/"

class FileIOManager
{
	DECLARE_SINGLE(FileIOManager);
public:
	~FileIOManager() = default;

	void Init();

public:
	void XMLFromMaterial(shared_ptr<Material> material, const wstring& name);

	void LoadTextures();
	void LoadMeshes();
	void LoadMaterials();
};

