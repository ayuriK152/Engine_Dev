#pragma once
class Geometry;
class Mesh;
class GameObject;
class MeshRenderer;

enum ModelFormat
{
	UNKOWN = -1,
	FBX,
	GLTF
};

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

public:
	void InitializeFields();

	void ReadAssetFile(wstring file);
	void ProcessMaterials(const aiScene* scene);
	void ProcessNodes(aiNode* node, const aiScene* scene, shared_ptr<Node> parentNode);
	void MapWeights();
	void MapBones();
	void BuildBones();
	shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

	shared_ptr<GameObject> GetLoadedObject() { return _loadedObject; }

	XMFLOAT4X4 ConvertToXMFLOAT4X4(aiMatrix4x4 mat) {
		XMFLOAT4X4 ret(
			mat.a1, mat.a2, mat.a3, mat.a4,
			mat.b1, mat.b2, mat.b3, mat.b4,
			mat.c1, mat.c2, mat.c3, mat.c4,
			mat.d1, mat.d2, mat.d3, mat.d4
		);
		return ret;
	}

private:
	wstring GetAIMaterialName(const aiScene* scene, UINT index);
	void ImportModelFormat(wstring fileName);

private:
	shared_ptr<Assimp::Importer> _importer;

	const aiScene* _scene;
	ModelFormat _modelType;
	wstring _assetName;

	map<string, shared_ptr<Node>> _nodes;
	map<string, shared_ptr<Bone>> _bones;
	vector<shared_ptr<GameObject>> _meshObjs;
	vector<shared_ptr<GameObject>> _boneObjs;
	vector<shared_ptr<MeshRenderer>> _meshRenderers;
	vector<shared_ptr<Mesh>> _meshes;

	shared_ptr<GameObject> _loadedObject;

	map<pair<int, string>, vector<BoneWeight>> _tempBoneWeights;

private:
	wstring _assetPath = L"../Resources/Assets/";
};

