#include "pch.h"
#include "AssetLoader.h"

AssetLoader::AssetLoader()
{
	_importer = make_shared<Assimp::Importer>();
}

AssetLoader::~AssetLoader()
{

}

void AssetLoader::InitializeFields()
{
	_nodes.clear();
	_bones.clear();
	_geometry.clear();
	_subMeshes.clear();
	_tempBoneWeights.clear();

	_submeshVertexOffset = 0;
	_submeshIndexOffset = 0;
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

	ImportModelFormat(file);

	_submeshVertexOffset = 0;
	_submeshIndexOffset = 0;
	auto anim = _scene->mAnimations;
	ProcessMaterials(_scene);
	ProcessNodes(_scene->mRootNode, _scene, nullptr);
	if (_bones.size() > 0)
	{
		MapWeights();
		MapBones();
		_mesh = make_shared<Mesh>(_subMeshes);
		_mesh->SetSkinnedMeshData(_nodes, _bones);
	}
	else
	{
		_mesh = make_shared<Mesh>(_subMeshes);
	}

	InitializeFields();
}

void AssetLoader::ProcessMaterials(const aiScene* scene)
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* aiMat = scene->mMaterials[i];
		int texCnt = aiMat->mNumProperties;
		aiMat->mProperties[0];
		string matNameStr(scene->mMaterials[i]->GetName().C_Str());
		wstring matName = GetAIMaterialName(scene, i);

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

void AssetLoader::ProcessNodes(aiNode* node, const aiScene* scene, shared_ptr<Node> parentNode)
{
	// 노드 저장
	shared_ptr<Node> currNode = make_shared<Node>();
	currNode->name = node->mName.C_Str();
	currNode->id = _nodes.size();
	currNode->parent = parentNode;
	currNode->transform = ConvertToXMFLOAT4X4(node->mTransformation);

	// 부모 노드 존재시 행렬 계산
	if (currNode->parent != nullptr)
	{
		XMMATRIX multipliedMat = XMLoadFloat4x4(&parentNode->transform) * XMLoadFloat4x4(&currNode->transform);
		XMStoreFloat4x4(&currNode->transform, multipliedMat);
	}
	
	_nodes.insert({ currNode->name, currNode });

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		// 메시 기하정보 로드
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		currNode->submeshIndices.push_back(_subMeshes.size());
		_subMeshes.push_back(ProcessMesh(mesh, scene));

		// 메시 본 로드 (있는 경우에만)
		if (mesh->HasBones())
		{
			for (int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* currentBone = mesh->mBones[i];
				// 본 중복 확인, 본 이름과 노드 검증
				if (!_bones.contains(currentBone->mName.C_Str()))
				{
					shared_ptr<Bone> bone = make_shared<Bone>();
					bone->name = mesh->mBones[i]->mName.C_Str();
					bone->id = _bones.size();
					bone->transform = ConvertToXMFLOAT4X4(mesh->mBones[i]->mOffsetMatrix);
					_bones.insert({ bone->name, bone });
				}
			}
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNodes(node->mChildren[i], scene, currNode);
	}
}

void AssetLoader::MapWeights()
{
	for (auto& weight : _tempBoneWeights)
	{
		int subMeshIndex = weight.first.first;
		string boneName = weight.first.second;
		UINT boneId = _bones[boneName]->id;
		shared_ptr<SubMesh> subMesh = _subMeshes[subMeshIndex];
		subMesh->SetWeights(boneId, weight.second);
	}
}

void AssetLoader::MapBones()
{
	for (auto bone = _bones.begin(); bone != _bones.end(); bone++)
	{
		shared_ptr<Node> node = _nodes[bone->first];
		if (node == nullptr)
			continue;
		bone->second->node = node;
	}
}

shared_ptr<SubMesh> AssetLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
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
		v.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		
		for (int j = 0; j < mesh->GetNumUVChannels(); j++)
		{
			switch (_modelType)
			{
			case ModelFormat::FBX:
				v.TexC = { (float)mesh->mTextureCoords[j][i].x, (float)mesh->mTextureCoords[j][i].y };
				break;
			case ModelFormat::GLTF:
				v.TexC = { (float)mesh->mTextureCoords[j][i].x, 1.0f - (float)mesh->mTextureCoords[j][i].y };
				break;
			}
		}

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

	// 본 가중치 여부 확인 후 추가
	if (mesh->HasBones())
	{
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			// 현재 서브메시에 대한 본별 가중치 임시 저장
			aiBone* currentBone = mesh->mBones[i];
			for (int j = 0; j < currentBone->mNumWeights; j++)
			{
				if (currentBone->mWeights[j].mWeight == 0)
					continue;

				BoneWeight weight;
				weight.vertexIndex = currentBone->mWeights[j].mVertexId;
				weight.weight = currentBone->mWeights[j].mWeight;
				_tempBoneWeights[{ _subMeshes.size(), currentBone->mName.C_Str() }].push_back(weight);
			}
		}
	}

	geometry->SetVertices(vertices);
	geometry->SetIndices(indices);
	geometry->SetVertexOffset(_submeshVertexOffset);
	geometry->SetIndexOffset(_submeshIndexOffset);
	_submeshVertexOffset += geometry->GetVertexCount();
	_submeshIndexOffset += geometry->GetIndexCount();

	shared_ptr<SubMesh> subMesh = make_shared<SubMesh>(geometry);
	subMesh->name = mesh->mName.C_Str();
	subMesh->id = _subMeshes.size();
	subMesh->SetMaterial(RESOURCE->Get<Material>(GetAIMaterialName(scene, mesh->mMaterialIndex)));
	return subMesh;
}
wstring AssetLoader::GetAIMaterialName(const aiScene* scene, UINT index)
{
	string matNameStr(scene->mMaterials[index]->GetName().C_Str());
	return UniversalUtils::ToWString(matNameStr);
}

void AssetLoader::ImportModelFormat(wstring fileName)
{
	istringstream ss(UniversalUtils::ToString(fileName));
	string format;
	while (getline(ss, format, '.'));

	if (format == "fbx")
		_modelType = ModelFormat::FBX;
	else if (format == "gltf")
		_modelType = ModelFormat::GLTF;
	else
		_modelType = ModelFormat::UNKOWN;
}
