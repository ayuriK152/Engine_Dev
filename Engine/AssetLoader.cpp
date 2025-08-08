#include "pch.h"
#include "AssetLoader.h"

AssetLoader::AssetLoader()
{

}

AssetLoader::~AssetLoader()
{

}

void AssetLoader::InitializeFields()
{
	_nodes.clear();
	_bones.clear();
	_meshObjs.clear();
	_boneObjs.clear();
	_tempBoneWeights.clear();
	_animations.clear();
}

// 바이너리 파일로 저장 안된경우 최초 임포트 메소드
void AssetLoader::ImportAssetFile(wstring file)
{
	_importer = make_shared<Assimp::Importer>();
	_importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);	// 자꾸 이상하고 쓸데 없는 노드 가져와서 설정함

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

	ImportModelFormat(UniversalUtils::ToWString(p.filename().string()));
	shared_ptr<GameObject> rootObj = make_shared<GameObject>();
	rootObj->GetName() = UniversalUtils::ToString(_assetName);
	_loadedObject.push_back(rootObj);

	ProcessMaterials(_scene);
	ProcessAnimation(_scene);
	ProcessNodes(_scene->mRootNode, _scene, nullptr);
	if (_bones.size() > 0)
	{
		MapWeights();
		MapBones();
		BuildBones();
	}

	// 바이너리 파일 저장
	{
		string assetNameStr = UniversalUtils::ToString(_assetName);

		for (auto& mesh : _meshes)
			RESOURCE->SaveMesh(mesh, assetNameStr + "\\" + mesh->GetName());

		for (auto& animation : _animations)
			RESOURCE->SaveAnimation(animation, assetNameStr + "\\" + animation->GetName());

		RESOURCE->SaveBone(_bones, assetNameStr);
		RESOURCE->SavePrefab(_loadedObject[0]);
	}

	InitializeFields();

	_importer = nullptr;
}

void AssetLoader::ProcessMaterials(const aiScene* scene)
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		wstring matName = GetAIMaterialName(scene, i);
		if (RESOURCE->Get<Material>(matName) != nullptr)
			continue;

		aiMaterial* aiMat = scene->mMaterials[i];
		string matNameStr(scene->mMaterials[i]->GetName().C_Str());

		shared_ptr<Material> mat = make_shared<Material>(matNameStr);

		aiColor4D color;
		aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
		mat->ambient = { color.r, color.g, color.b, color.a };
		aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		mat->diffuse = { color.r, color.g, color.b, color.a };
		aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		mat->specular = { color.r, color.g, color.b, color.a };
		aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		mat->emissive = { color.r, color.g, color.b, color.a };

		// 텍스쳐 로드부분 수정해야함
		/*
		* 1. 모델 파일 자체에 텍스쳐 정보가 있는지 확인
		* 2. 없다면 머터리얼과 같은 이름의 텍스쳐 파일이 있는지 확인
		* 3. 없다면 기본 생성자 적용
		*
		* 단, 텍스쳐 종류가 여러 가지 있는듯 하니 어떻게 적용할지 계획을 세워서 구현할 것
		*/
		if (Texture::IsTextureExists(matName + L".dds"))
		{
			shared_ptr<Texture> texture = make_shared<Texture>(matName + L".dds");
			RESOURCE->Add<Texture>(matName, texture);
			mat->SetTexture(RESOURCE->Get<Texture>(matName));
		}
		RESOURCE->Add<Material>(matName, mat);

		FILEIO->XMLFromMaterial(mat, _assetName);
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
		XMMATRIX multipliedMat = XMLoadFloat4x4(&currNode->transform) * XMLoadFloat4x4(&parentNode->transform);
		XMStoreFloat4x4(&currNode->transform, multipliedMat);
	}

	_nodes.insert({ currNode->name, currNode });

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		// 메시 기하정보 로드
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		shared_ptr<Mesh> m = RESOURCE->Get<Mesh>(UniversalUtils::ToWString(mesh->mName.C_Str()));
		if (m == nullptr)
		{
			m = ProcessMesh(mesh, scene);
			RESOURCE->Add<Mesh>(UniversalUtils::ToWString(node->mName.C_Str()), m);
		}
		_meshes.push_back(m);

		// 본 없는 경우에는 그냥 MeshRenderer로 하도록 변경 필요
		shared_ptr<GameObject> meshObj = make_shared<GameObject>();
		meshObj->GetName() = UniversalUtils::ToString(m->GetNameW());
		meshObj->AddComponent(make_shared<SkinnedMeshRenderer>());
		meshObj->GetComponent<SkinnedMeshRenderer>()->SetMesh(m);
		meshObj->GetTransform()->SetParent(_loadedObject[0]->GetTransform());
		_meshObjs.push_back(meshObj);
		_loadedObject.push_back(meshObj);

		// 메시 본 로드 (있는 경우에만)
		if (mesh->HasBones())
		{
			for (int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* currentBone = mesh->mBones[i];
				// 본 중복 확인, 본 이름과 노드 검증
				if (!_bones.contains(currentBone->mName.C_Str()))
				{
					Bone bone;
					bone.name = mesh->mBones[i]->mName.C_Str();
					bone.id = _bones.size();
					bone.offsetTransform = ConvertToXMFLOAT4X4(mesh->mBones[i]->mOffsetMatrix);

					_bones.insert({ bone.name, bone });
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
		UINT boneId = _bones[boneName].id;
		shared_ptr<Mesh> mesh = _meshes[subMeshIndex];
		mesh->SetWeights(boneId, weight.second);
	}
}

void AssetLoader::MapBones()
{
	for (auto bone = _bones.begin(); bone != _bones.end(); bone++)
	{
		shared_ptr<Node> node = _nodes[bone->first];
		if (node == nullptr)
			continue;
		bone->second.node = node;
	}
}

void AssetLoader::BuildBones()
{
	vector<Bone> sortedBones;
	sortedBones.reserve(_bones.size());
	for (auto& b : _bones)
		sortedBones.push_back(b.second);

	sort(sortedBones.begin(), sortedBones.end(), [](Bone a, Bone b) { return a.id < b.id; });

	for (auto& b : sortedBones)
	{
		shared_ptr<GameObject> foundObj = nullptr;
		shared_ptr<Node> currentParent = b.node->parent;

		while (true)
		{
			if (currentParent == nullptr)
				break;
			if (_bones.contains(currentParent->name))
			{
				foundObj = _bones[currentParent->name].instancedObj;
				break;
			}
			currentParent = currentParent->parent;
		}

		shared_ptr<GameObject> boneObj = make_shared<GameObject>();
		boneObj->GetName() = b.name;

		if (foundObj != nullptr)
			boneObj->GetTransform()->SetParent(foundObj->GetTransform());
		boneObj->GetTransform()->SetWorldMatrix(b.node->transform);

		_boneObjs.push_back(boneObj);
		_loadedObject.push_back(boneObj);
		_bones[b.name].instancedObj = boneObj;
	}

	for (auto& meshObj : _meshObjs)
	{
		auto renderer = meshObj->GetComponent<SkinnedMeshRenderer>();
		renderer->SetBoneData(_bones);
		renderer->SetRootBone(_boneObjs[0]->GetTransform());
	}

	_boneObjs[0]->GetTransform()->SetParent(_loadedObject[0]->GetTransform());
	assert(_boneObjs.size() != 0);
}

shared_ptr<Mesh> AssetLoader::ProcessMesh(aiMesh* aimesh, const aiScene* scene)
{
	shared_ptr<Geometry> geometry = make_shared<Geometry>();

	vector<Vertex> vertices;
	vector<UINT16> indices;

	// Get Vertices
	vertices.reserve(aimesh->mNumVertices);
	for (UINT i = 0; i < aimesh->mNumVertices; i++)
	{
		Vertex v;
		v.Position = { aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z };
		if (aimesh->HasNormals())
			v.Normal = { aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z };
		
		for (int j = 0; j < aimesh->GetNumUVChannels(); j++)
		{
			switch (_modelType)
			{
			case ModelFormat::FBX:
				v.TexC = { (float)aimesh->mTextureCoords[j][i].x, (float)aimesh->mTextureCoords[j][i].y };
				break;
			case ModelFormat::GLTF:
				v.TexC = { (float)aimesh->mTextureCoords[j][i].x, 1.0f - (float)aimesh->mTextureCoords[j][i].y };
				break;
			}
		}

		vertices.push_back(v);
	}

	// Get Indices
	UINT numIndices = 0;
	for (UINT i = 0; i < aimesh->mNumFaces; i++)
	{
		numIndices += aimesh->mFaces[i].mNumIndices;
	}

	indices.reserve(numIndices);
	for (UINT i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// 본 가중치 여부 확인 후 추가
	if (aimesh->HasBones())
	{
		for (int i = 0; i < aimesh->mNumBones; i++)
		{
			// 현재 서브메시에 대한 본별 가중치 임시 저장
			aiBone* currentBone = aimesh->mBones[i];
			for (int j = 0; j < currentBone->mNumWeights; j++)
			{
				if (currentBone->mWeights[j].mWeight == 0)
					continue;

				BoneWeight weight;
				weight.vertexIndex = currentBone->mWeights[j].mVertexId;
				weight.weight = currentBone->mWeights[j].mWeight;
				_tempBoneWeights[{ _meshes.size(), currentBone->mName.C_Str() }].push_back(weight);
			}
		}
	}

	geometry->SetVertices(vertices);
	geometry->SetIndices(indices);

	shared_ptr<Mesh> mesh = make_shared<Mesh>(geometry);
	mesh->SetName(UniversalUtils::ToWString(aimesh->mName.C_Str()));
	auto mat = RESOURCE->Get<Material>(GetAIMaterialName(scene, aimesh->mMaterialIndex));
	mesh->SetMaterial(mat);
	return mesh;
}


// 애니메이션 데이터 추출
void AssetLoader::ProcessAnimation(const aiScene* scene)
{
	if (!scene->HasAnimations())
		return;

	_loadedObject[0]->AddComponent(make_shared<Animator>());

	// 애니메이션 갯수만큼
	for (int i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];
		shared_ptr<Animation> bAnim = make_shared<Animation>(anim->mName.C_Str(), anim->mDuration, anim->mTicksPerSecond);

		for (int j = 0; j < anim->mNumChannels; j++)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			string name = channel->mNodeName.data;

			Animation::AnimationData animData;
			animData.boneName = name;

			UINT maxKeyCount = max({ channel->mNumPositionKeys, channel->mNumRotationKeys, channel->mNumScalingKeys });

			// position, rotation, scale 키프레임 받아오기
			map<float, Animation::KeyFrame> keyframeMap;
			for (int k = 0; k < maxKeyCount; k++)
			{
				if (k < channel->mNumPositionKeys)
				{
					aiVectorKey pos = channel->mPositionKeys[k];
					if (!keyframeMap.contains(pos.mTime))
						keyframeMap[pos.mTime].tick = pos.mTime;
					keyframeMap[pos.mTime].position = { pos.mValue.x, pos.mValue.y, pos.mValue.z };
				}
				
				if (k < channel->mNumRotationKeys)
				{
					aiQuatKey rot = channel->mRotationKeys[k];
					if (!keyframeMap.contains(rot.mTime))
						keyframeMap[rot.mTime].tick = rot.mTime;
					keyframeMap[rot.mTime].rotation = { rot.mValue.x, rot.mValue.y, rot.mValue.z, rot.mValue.w };
				}

				if (k < channel->mNumScalingKeys)
				{
					aiVectorKey scale = channel->mScalingKeys[k];
					if (!keyframeMap.contains(scale.mTime))
						keyframeMap[scale.mTime].tick = scale.mTime;
					keyframeMap[scale.mTime].scale = { scale.mValue.x, scale.mValue.y, scale.mValue.z };
				}
			}

			for (auto& kf : keyframeMap)
				animData.keyFrames.push_back(kf.second);

			bAnim->AddAnimationData(animData);
		}
		_animations.push_back(bAnim);
		_loadedObject[0]->GetComponent<Animator>()->AddAnimation(bAnim);
	}
}

wstring AssetLoader::GetAIMaterialName(const aiScene* scene, UINT index)
{
	string matNameStr(scene->mMaterials[index]->GetName().C_Str());
	return UniversalUtils::ToWString(matNameStr);
}

void AssetLoader::ImportModelFormat(wstring fileName)
{
	_assetName = fileName;
	_assetName.erase(_assetName.find_last_of(L"."), wstring::npos);

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
