#include "pch.h"
#include "AssetLoader.h"

AssetLoader::AssetLoader()
{
	_importer = make_shared<Assimp::Importer>();
}

AssetLoader::~AssetLoader()
{

}

void AssetLoader::ReadAssetFile(wstring file)
{
	wstring fileStr = _assetPath + file;

	auto p = filesystem::path(fileStr);
	assert(filesystem::exists(p));

	_scene = _importer->ReadFile(
		UniversalUtils::ToString(fileStr),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	assert(_scene != nullptr);

	_submeshVertexOffset = 0;
	_submeshIndexOffset = 0;
	ProcessMaterials(_scene);
	ProcessNodes(_scene->mRootNode, _scene);
	_mesh = make_shared<Mesh>(_geometry);
}

void AssetLoader::ProcessMaterials(const aiScene* scene)
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		wstring matName;

		string matNameStr(scene->mMaterials[i]->GetName().C_Str());
		matName = UniversalUtils::ToWString(matNameStr);

		shared_ptr<Material> mat = make_shared<Material>(matNameStr, 0, 0, -1);
		if (Texture::IsTextureExists(matName + L".dds"))
		{
			shared_ptr<Texture> texture = make_shared<Texture>(matName + L".dds");
			RESOURCE->Add<Texture>(matName, texture);
			mat->SetTexture(RESOURCE->Get<Texture>(matName));
		}
		RESOURCE->Add<Material>(matName, mat);
	}
}

void AssetLoader::ProcessNodes(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_geometry.push_back(ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNodes(node->mChildren[i], scene);
	}
}

shared_ptr<Geometry> AssetLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	shared_ptr<Geometry> geometry = make_shared<Geometry>();

	vector<Vertex> vertices;
	vector<UINT16> indices;

	// Get Vertices
	vertices.reserve(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;
		v.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		
		if (mesh->mTextureCoords[0])
			v.TexC = {(float)mesh->mTextureCoords[0][i].x, (float)mesh->mTextureCoords[0][i].y};
		vertices.push_back(v);
	}

	// Get Indices
	UINT numIndices = 0;
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		numIndices += mesh->mFaces[i].mNumIndices;
	}

	indices.reserve(numIndices);
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	geometry->SetVertices(vertices);
	geometry->SetIndices(indices);
	geometry->SetVertexOffset(_submeshVertexOffset);
	geometry->SetIndexOffset(_submeshIndexOffset);
	_submeshVertexOffset += geometry->GetVertexCount();
	_submeshIndexOffset += geometry->GetIndexCount();
	return geometry;
}