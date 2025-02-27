#pragma once

class FbxLoader
{
public:
	FbxLoader();
	~FbxLoader();

	bool Initialize();
	bool Load(const char* fileName);

	XMFLOAT3* GetVertexPos() { return _pos; }
	UINT* GetVertexIdx() { return _idx; }

private:
	bool FindMesh(FbxNode* node);
	bool SaveVertexData();

private:
	FbxManager* _fbxManager;
	FbxImporter* _fbxImporter;
	FbxScene* _fbxScene;
	FbxMesh* _fbxMesh;

	XMFLOAT3* _pos;
	UINT* _idx;
};

