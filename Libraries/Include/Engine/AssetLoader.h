#pragma once

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

public:
	void ReadAssetFile(wstring file);

private:
	shared_ptr<Assimp::Importer> _importer;

	const aiScene* _scene;

private:
	wstring _assetPath = L"../Resources/Assets/";
};

