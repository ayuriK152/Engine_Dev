#pragma once
class Geometry;
class Mesh;

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

public:
	void ReadAssetFile(wstring file);
	void ProcessMaterials(const aiScene* scene);
	void ProcessNodes(aiNode* node, const aiScene* scene);
	shared_ptr<Geometry> ProcessMesh(aiMesh* mesh, const aiScene* scene);

	shared_ptr<Mesh> GetLoadedMesh() { return _mesh; }

private:
	shared_ptr<Assimp::Importer> _importer;

	const aiScene* _scene;
	vector<shared_ptr<Geometry>> _geometry;
	shared_ptr<Mesh> _mesh;

	UINT _submeshVertexOffset = 0;
	UINT _submeshIndexOffset = 0;

private:
	wstring _assetPath = L"../Resources/Assets/";
};

