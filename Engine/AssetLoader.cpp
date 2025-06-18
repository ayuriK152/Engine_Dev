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
	_loadedObject = make_shared<GameObject>();
	_loadedObject->name = UniversalUtils::ToString(_assetName);

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
	SaveMeshData();
	SaveAnimationData();
	SaveBoneData();
	SavePrefabData();

	ReadMeshData("Alpha_Joints");
	ReadAnimationData("mixamo.com");
	ReadBoneData("Y Bot");
	ReadPrefabObject("Y Bot");

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
		meshObj->name = UniversalUtils::ToString(m->GetNameW());
		meshObj->AddComponent(make_shared<SkinnedMeshRenderer>());
		meshObj->GetComponent<SkinnedMeshRenderer>()->SetMesh(m);
		meshObj->GetTransform()->SetParent(_loadedObject->GetTransform());
		_meshObjs.push_back(meshObj);

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
					bone->offsetTransform = ConvertToXMFLOAT4X4(mesh->mBones[i]->mOffsetMatrix);

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
		bone->second->node = node;
	}
}

void AssetLoader::BuildBones()
{
	vector<shared_ptr<Bone>> sortedBones;
	sortedBones.reserve(_bones.size());
	for (auto& b : _bones)
		sortedBones.push_back(b.second);

	sort(sortedBones.begin(), sortedBones.end(), [](shared_ptr<Bone> a, shared_ptr<Bone> b) { return a->id < b->id; });

	for (auto& b : sortedBones)
	{
		shared_ptr<GameObject> foundObj = nullptr;
		shared_ptr<Node> currentParent = b->node->parent;
		while (true)
		{
			if (currentParent == nullptr)
				break;
			if (_bones.contains(currentParent->name))
			{
				foundObj = _bones[currentParent->name]->instancedObj;
				break;
			}
			currentParent = currentParent->parent;
		}
		shared_ptr<GameObject> boneObj = make_shared<GameObject>();
		boneObj->name = b->name;
		XMFLOAT4X4 finalTransform;
		XMStoreFloat4x4(&finalTransform, XMMatrixMultiply(XMLoadFloat4x4(&b->offsetTransform), XMLoadFloat4x4(&b->node->transform)));

		if (foundObj != nullptr)
			boneObj->GetTransform()->SetParent(foundObj->GetTransform());
		boneObj->GetTransform()->SetObjectWorldMatrix(b->node->transform);
		_boneObjs.push_back(boneObj);
		b->instancedObj = boneObj;

	}

	for (auto& meshObj : _meshObjs)
	{
		auto renderer = meshObj->GetComponent<SkinnedMeshRenderer>();
		renderer->SetBoneData(_bones);
		renderer->SetRootBone(_boneObjs[0]->GetTransform());
	}

	_boneObjs[0]->GetTransform()->SetParent(_loadedObject->GetTransform());
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

	_loadedObject->AddComponent(make_shared<Animator>());

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
		_loadedObject->GetComponent<Animator>()->AddAnimation(bAnim);
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

void AssetLoader::SaveMeshData()
{
	for (shared_ptr<Mesh> mesh : _meshes)
	{
		HANDLE fileHandle = FILEIO->CreateFileHandle<Mesh>(UniversalUtils::ToString(mesh->GetNameW()));

		UINT32 vertexCount = mesh->GetVertexCount();
		FILEIO->WriteToFile(fileHandle, vertexCount);
		for (const Vertex& v : mesh->GetVertices())
		{
			FILEIO->WriteToFile(fileHandle, v);
		}

		UINT32 indexCount = mesh->GetIndexCount();
		FILEIO->WriteToFile(fileHandle, indexCount);
		for (UINT16 i : mesh->GetIndices())
		{
			FILEIO->WriteToFile(fileHandle, i);
		}

		CloseHandle(fileHandle);
	}
}

void AssetLoader::SaveAnimationData()
{
	UINT32 animCount = _animations.size();
	for (int i = 0; i < animCount; i++)
	{
		HANDLE fileHandle = FILEIO->CreateFileHandle<Animation>(_animations[i]->GetName());

		FILEIO->WriteToFile(fileHandle, _animations[i]->GetDuration());
		FILEIO->WriteToFile(fileHandle, _animations[i]->GetTicksPerSecond());

		unordered_map<string, Animation::AnimationData> animationDatas = _animations[i]->GetAnimationDatas();
		FILEIO->WriteToFile(fileHandle, (UINT32)animationDatas.size());

		for (auto& animData : animationDatas)
		{
			FILEIO->WriteToFile(fileHandle, animData.first);
			UINT32 keyFrameCount = animData.second.keyFrames.size();
			FILEIO->WriteToFile(fileHandle, keyFrameCount);
			for (auto& keyFrame : animData.second.keyFrames)
			{
				FILEIO->WriteToFile(fileHandle, keyFrame);
			}
		}

		CloseHandle(fileHandle);
	}
}

void AssetLoader::SaveBoneData()
{
	HANDLE fileHandle = FILEIO->CreateFileHandle<Bone>(UniversalUtils::ToString(_assetName));
	UINT32 boneCount = _bones.size();

	FILEIO->WriteToFile(fileHandle, boneCount);
	for (auto& bone : _bones)
	{
		FILEIO->WriteToFile(fileHandle, bone.second->name);
		FILEIO->WriteToFile(fileHandle, bone.second->id);
		FILEIO->WriteToFile(fileHandle, bone.second->offsetTransform);
	}
	CloseHandle(fileHandle);
}

void AssetLoader::SavePrefabData()
{
	vector<shared_ptr<Transform>> allObjects;
	allObjects.push_back(_loadedObject->GetTransform());
	for (int i = 0; i < allObjects.size(); i++)
	{
		allObjects.insert(allObjects.end(), allObjects[i]->GetChilds().begin(), allObjects[i]->GetChilds().end());
	}

	HANDLE fileHandle = FILEIO->CreateFileHandle<GameObject>(UniversalUtils::ToString(_assetName));
	FILEIO->WriteToFile(fileHandle, (UINT32)allObjects.size());
	WritePrefabRecursive(fileHandle, _loadedObject, -1);
	CloseHandle(fileHandle);
}

void AssetLoader::WritePrefabRecursive(HANDLE fileHandle, shared_ptr<GameObject> obj, int parentIdx)
{
	static int objectIdx = 0;
	int currentIdx = objectIdx++;

	FILEIO->WriteToFile(fileHandle, obj->name);
	FILEIO->WriteToFile(fileHandle, obj->psoName);
	FILEIO->WriteToFile(fileHandle, obj->GetTransform()->GetWorldMatrix());
	FILEIO->WriteToFile(fileHandle, parentIdx);		// 부모 인덱스
	FILEIO->WriteToFile(fileHandle, (UINT32)obj->GetComponents().size() - 1);
	for (auto& c : obj->GetComponents())
	{
		if (c.first == ComponentType::Transform)
			continue;

		FILEIO->WriteToFile(fileHandle, c.first);	// 컴포넌트 타입

		switch (c.first)
		{
			case ComponentType::MeshRenderer:
			{
				auto meshRenderer = static_pointer_cast<MeshRenderer>(c.second);

				string meshName = UniversalUtils::ToString(meshRenderer->GetMesh()->GetNameW());
				FILEIO->WriteToFile(fileHandle, meshName);

				string matName = UniversalUtils::ToString(meshRenderer->GetMaterial()->GetNameW());
				FILEIO->WriteToFile(fileHandle, matName);

				break;
			}

			case ComponentType::SkinnedMeshRenderer:
			{
				auto meshRenderer = static_pointer_cast<SkinnedMeshRenderer>(c.second);

				string meshName = UniversalUtils::ToString(meshRenderer->GetMesh()->GetNameW());
				FILEIO->WriteToFile(fileHandle, meshName);

				string matName = UniversalUtils::ToString(meshRenderer->GetMaterial()->GetNameW());
				FILEIO->WriteToFile(fileHandle, matName);

				string rootBoneName = meshRenderer->GetRootBone()->GetGameObject()->name;
				FILEIO->WriteToFile(fileHandle, rootBoneName);

				string boneDataName = UniversalUtils::ToString(_assetName);
				FILEIO->WriteToFile(fileHandle, boneDataName);

				break;
			}

			case ComponentType::Animator:
			{
				auto animator = static_pointer_cast<Animator>(c.second);

				FILEIO->WriteToFile(fileHandle, animator->IsPlayOnInit());
				FILEIO->WriteToFile(fileHandle, animator->IsLoop());

				string currentAnimName = animator->GetCurrentAnimation()->GetName();
				FILEIO->WriteToFile(fileHandle, currentAnimName);

				auto animations = animator->GetAnimations();
				FILEIO->WriteToFile(fileHandle, (UINT32)animations.size());
				for (auto& anim : animations)
				{
					/* 현재는 애니메이션의 이름만 저장함. 
					*  나중에 Resource 전체 리팩토링을 진행하면서 전체 경로를 저장하는 방식을 고려할 수 있도록
					*/
					FILEIO->WriteToFile(fileHandle, anim.first);
				}

				break;
			}
		}
	}

	for (auto& c : obj->GetTransform()->GetChilds())
	{
		WritePrefabRecursive(fileHandle, c->GetGameObject(), currentIdx);
	}
}

shared_ptr<Mesh> AssetLoader::ReadMeshData(string fileName)
{
	HANDLE fileHandle = FILEIO->CreateFileHandle<Mesh>(fileName);

	UINT32 vertexCount;
	FILEIO->ReadFileData(fileHandle, &vertexCount, sizeof(UINT32));
	vector<Vertex> vertices;
	for (int i = 0; i < vertexCount; i++)
	{
		Vertex v;
		FILEIO->ReadFileData(fileHandle, &v, sizeof(Vertex));

		vertices.push_back(v);
	}

	UINT32 indexCount;
	FILEIO->ReadFileData(fileHandle, &indexCount, sizeof(UINT32));
	vector<UINT16> indices;
	for (int i = 0; i < indexCount; i++)
	{
		UINT16 index;
		FILEIO->ReadFileData(fileHandle, &index, sizeof(UINT16));
		indices.push_back(index);
	}

	CloseHandle(fileHandle);

	shared_ptr<Geometry> geometry = make_shared<Geometry>(vertices, indices);
	shared_ptr<Mesh> loadedMesh = make_shared<Mesh>(geometry);

	return loadedMesh;
}

shared_ptr<Animation> AssetLoader::ReadAnimationData(string fileName)
{
	HANDLE fileHandle = FILEIO->CreateFileHandle<Animation>(fileName);
	float duration, ticks;
	FILEIO->ReadFileData(fileHandle, &duration, sizeof(float));
	FILEIO->ReadFileData(fileHandle, &ticks, sizeof(float));

	UINT32 animationDataCount;
	FILEIO->ReadFileData(fileHandle, &animationDataCount, sizeof(UINT32));
	unordered_map<string, Animation::AnimationData> animationDatas;
	for (int i = 0; i < animationDataCount; i++)
	{
		string objName;
		FILEIO->ReadFileData(fileHandle, objName);
		animationDatas[objName].boneName = objName;

		UINT32 keyFrameCount;
		FILEIO->ReadFileData(fileHandle, &keyFrameCount, sizeof(UINT32));

		for (int j = 0; j < keyFrameCount; j++)
		{
			Animation::KeyFrame keyFrame;
			FILEIO->ReadFileData(fileHandle, &keyFrame, sizeof(Animation::KeyFrame));
			animationDatas[objName].keyFrames.push_back(keyFrame);
		}
	}

	CloseHandle(fileHandle);

	shared_ptr<Animation> loadedAnimation = make_shared<Animation>(fileName, duration, ticks);

	return loadedAnimation;
}

map<string, Bone> AssetLoader::ReadBoneData(string fileName)
{
	map<string, Bone> boneData;

	HANDLE fileHandle = FILEIO->CreateFileHandle<Bone>(fileName);
	UINT32 boneCount;

	FILEIO->ReadFileData(fileHandle, &boneCount, sizeof(UINT32));
	for (int i = 0; i < boneCount; i++)
	{
		Bone bone;
		FILEIO->ReadFileData(fileHandle, bone.name);
		FILEIO->ReadFileData(fileHandle, &bone.id, sizeof(UINT));
		FILEIO->ReadFileData(fileHandle, &bone.offsetTransform, sizeof(XMFLOAT4X4));
		boneData[bone.name] = bone;
	}
	CloseHandle(fileHandle);

	return boneData;
}

vector<shared_ptr<GameObject>> AssetLoader::ReadPrefabObject(string fileName)
{
	HANDLE fileHandle = FILEIO->CreateFileHandle<GameObject>(fileName);

	vector<shared_ptr<GameObject>> loadedObjects;

	UINT32 objectCount;
	FILEIO->ReadFileData(fileHandle, &objectCount, sizeof(UINT32));
	for (int i = 0; i < objectCount; i++)
	{
		shared_ptr<GameObject> go = make_shared<GameObject>();
		FILEIO->ReadFileData(fileHandle, go->name);
		FILEIO->ReadFileData(fileHandle, go->psoName);

		XMFLOAT4X4 worldMat;
		FILEIO->ReadFileData(fileHandle, &worldMat, sizeof(XMFLOAT4X4));
		go->GetTransform()->SetObjectWorldMatrix(worldMat);

		int parentIdx;
		FILEIO->ReadFileData(fileHandle, &parentIdx, sizeof(int));

		UINT32 componentCount;
		FILEIO->ReadFileData(fileHandle, &componentCount, sizeof(UINT32));

		for (int j = 0; j < componentCount; j++)
		{
			ComponentType componentType;
			FILEIO->ReadFileData(fileHandle, &componentType, sizeof(ComponentType));

			switch (componentType)
			{
				case ComponentType::MeshRenderer:
				{
					shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();

					string meshName;
					FILEIO->ReadFileData(fileHandle, meshName);

					string matName;
					FILEIO->ReadFileData(fileHandle, matName);

					go->AddComponent(meshRenderer);

					break;
				}

				case ComponentType::SkinnedMeshRenderer:
				{
					shared_ptr<MeshRenderer> meshRenderer = make_shared<SkinnedMeshRenderer>();

					string meshName;
					FILEIO->ReadFileData(fileHandle, meshName);

					string matName;
					FILEIO->ReadFileData(fileHandle, matName);

					string rootBoneName;
					FILEIO->ReadFileData(fileHandle, rootBoneName);

					string boneDataName;
					FILEIO->ReadFileData(fileHandle, boneDataName);

					go->AddComponent(meshRenderer);

					break;
				}

				case ComponentType::Animator:
				{
					shared_ptr<Animator> animator = make_shared<Animator>();

					bool isPlayOnInit, isLoop;
					FILEIO->ReadFileData(fileHandle, &isPlayOnInit, sizeof(bool));
					FILEIO->ReadFileData(fileHandle, &isLoop, sizeof(bool));

					string currentAnimName;
					FILEIO->ReadFileData(fileHandle, currentAnimName);

					UINT32 animationCount;
					FILEIO->ReadFileData(fileHandle, &animationCount, sizeof(UINT32));

					vector<string> animationNames;
					for (int k = 0; k < animationCount; k++)
					{
						string animationName;
						FILEIO->ReadFileData(fileHandle, animationName);
						animationNames.push_back(animationName);
					}

					animator->SetPlayOnInit(isPlayOnInit);
					animator->SetLoop(isLoop);
					// animationNames로부터 애니메이션 맵 먼저 로드 해야지 안터짐. 구현 필요.
					animator->SetCurrentAnimation(currentAnimName);

					go->AddComponent(animator);

					break;
				}
			}
		}
	}

	return loadedObjects;
}
