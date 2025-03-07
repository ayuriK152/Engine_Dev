#pragma once
class Geometry;

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

public:
	void ReadAssetFile(wstring file);
	void ProcessNodes(aiNode* node, const aiScene* scene);
	shared_ptr<Geometry> ProcessMesh(aiMesh* mesh, const aiScene* scene);

private:
	shared_ptr<Assimp::Importer> _importer;

	const aiScene* _scene;
	vector<shared_ptr<Geometry>> _geometrys;

private:
	wstring _assetPath = L"../Resources/Assets/";
};

