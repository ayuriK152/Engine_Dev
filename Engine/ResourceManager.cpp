#include "pch.h"
#include "ResourceManager.h"

bool ResourceManager::CheckResourceExists(const string& filePath)
{
	return _resourcePaths.find(filePath) != _resourcePaths.end();
}

void ResourceManager::CreateDefaultResources()
{
	//==========Shader==========
	D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1", NULL, NULL
	};

	// opaque shaders
	auto stdVS = make_shared<Shader>(L"DefaultVS.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"standardVS", stdVS);
	auto skinnedVS = make_shared<Shader>(L"DefaultVS.hlsl", skinnedDefines, ShaderType::VS);
	Add<Shader>(L"skinnedVS", skinnedVS);
	auto opaquePS = make_shared<Shader>(L"DefaultPS.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"opaquePS", opaquePS);

	// skybox shaders
	auto skyboxVS = make_shared<Shader>(L"SkyboxVS.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"skyboxVS", skyboxVS);
	auto skyboxPS = make_shared<Shader>(L"SkyboxPS.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"skyboxPS", skyboxPS);

	// shadowmap shaders
	auto shadowVS = make_shared<Shader>(L"ShadowVS.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"shadowVS", shadowVS);
	auto skinnedShadowVS = make_shared<Shader>(L"ShadowVS.hlsl", skinnedDefines, ShaderType::VS);
	Add<Shader>(L"skinnedShadowVS", skinnedShadowVS);
	auto shadowPS = make_shared<Shader>(L"ShadowPS.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"shadowPS", shadowPS);

	// collider debug shaders
	auto debugVS = make_shared<Shader>(L"DebugVS.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"debugVS", debugVS);
	auto debugPS = make_shared<Shader>(L"DebugPS.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"debugPS", debugPS);

	// particle shaders
	auto particleCS = make_shared<Shader>(L"ParticleCS.hlsl", nullptr, ShaderType::CS);
	Add<Shader>(L"particleCS", particleCS);
	auto particleVS = make_shared<Shader>(L"ParticleVS.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"particleVS", particleVS);
	auto particleGS = make_shared<Shader>(L"ParticleGS.hlsl", nullptr, ShaderType::GS);
	Add<Shader>(L"particleGS", particleGS);
	auto particlePS = make_shared<Shader>(L"ParticlePS.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"particlePS", particlePS);


	//==========Texture==========
	auto defaultTex = make_shared<Texture>(L"white1x1.dds", true);
	Add<Texture>(L"Tex_Default", defaultTex);

	auto skyboxTex = make_shared<Texture>(L"Skybox_Daylight.dds", TextureType::Skybox, true);
	Add<Texture>(L"Tex_DefaultSkybox", skyboxTex);


	//==========Material==========
	auto defaultMat = make_shared<Material>("Default");
	Add<Material>(L"Mat_Default", defaultMat);

	auto defaultSkyboxMat = make_shared<Material>("DefaultSkybox", L"Tex_DefaultSkybox");
	Add<Material>(L"Mat_DefaultSkybox", defaultSkyboxMat);


	//==========Mesh==========
	shared_ptr<Mesh> boxMesh = make_shared<Mesh>(GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, 1));
	boxMesh->SetName("BasicBox");
	Add<Mesh>(L"Mesh_BasicBox", boxMesh);

	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>(GeometryGenerator::CreateGeosphere(0.5f, 3));
	sphereMesh->SetName("BasicSphere");
	Add<Mesh>(L"Mesh_BasicSphere", sphereMesh);

	shared_ptr<Mesh> quadMesh = make_shared<Mesh>(GeometryGenerator::CreateQuad());
	quadMesh->SetName("BasicQuad");
	Add<Mesh>(L"Mesh_BasicQuad", quadMesh);

	shared_ptr<Mesh> skyboxSphereMesh = make_shared<Mesh>(GeometryGenerator::CreateGeosphere(0.5f, 1));
	skyboxSphereMesh->SetName("SkyboxSphere");
	Add<Mesh>(L"Mesh_SkyboxSphere", skyboxSphereMesh);
}

void ResourceManager::SaveMesh(shared_ptr<Mesh> mesh, const string& filePath)
{
	string finalPath = filePath == "" ? mesh->GetName() : filePath;
	mesh->SetPath(RESOURCE_PATH_MESH + finalPath + BULB_EXT_MESH);
	HANDLE fileHandle = FILEIO->CreateFileHandle<Mesh>(finalPath);

	UINT32 fileVersion = FILE_VERSION_MESH;
	FILEIO->WriteToFile(fileHandle, fileVersion);

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

	string matName = mesh->GetMaterial()->GetName();
	FILEIO->WriteToFile(fileHandle, matName);

	CloseHandle(fileHandle);
}

void ResourceManager::SaveAnimation(shared_ptr<Animation> animation, const string& filePath)
{
	string finalPath = filePath == "" ? animation->GetName() : filePath;
	animation->SetPath(RESOURCE_PATH_ANIMATION + finalPath + BULB_EXT_ANIMATION);
	HANDLE fileHandle = FILEIO->CreateFileHandle<Animation>(finalPath);

	UINT32 fileVersion = FILE_VERSION_ANIM;
	FILEIO->WriteToFile(fileHandle, fileVersion);

	FILEIO->WriteToFile(fileHandle, animation->GetDuration());
	FILEIO->WriteToFile(fileHandle, animation->GetTicksPerSecond());

	vector<Animation::AnimationData> animationDatas = animation->GetAnimationDatas();
	FILEIO->WriteToFile(fileHandle, (UINT32)animationDatas.size());

	for (auto& animData : animationDatas)
	{
		FILEIO->WriteToFile(fileHandle, animData.boneName);

		UINT32 boneId = animData.boneId;
		FILEIO->WriteToFile(fileHandle, boneId);

		UINT32 keyFrameCount = animData.keyFrames.size();
		FILEIO->WriteToFile(fileHandle, keyFrameCount);
		for (auto& keyFrame : animData.keyFrames)
		{
			FILEIO->WriteToFile(fileHandle, keyFrame);
		}
	}

	CloseHandle(fileHandle);
}

void ResourceManager::SaveBone(map<string, BoneData> bones, const string& boneName, const string& filePath)
{
	string finalPath = filePath == "" ? boneName : filePath;
	HANDLE fileHandle = FILEIO->CreateFileHandle<BoneData>(finalPath);

	UINT32 fileVersion = FILE_VERSION_BONE;
	FILEIO->WriteToFile(fileHandle, fileVersion);

	UINT32 boneCount = bones.size();
	FILEIO->WriteToFile(fileHandle, boneCount);

	for (auto& bone : bones)
	{
		FILEIO->WriteToFile(fileHandle, bone.second.name);
		FILEIO->WriteToFile(fileHandle, bone.second.id);
		FILEIO->WriteToFile(fileHandle, bone.second.parentId);
		FILEIO->WriteToFile(fileHandle, bone.second.offsetTransform);
		FILEIO->WriteToFile(fileHandle, bone.second.localBindTransform);
	}
	CloseHandle(fileHandle);
}

void ResourceManager::SavePrefab(shared_ptr<GameObject> prefabObject, const string& filePath)
{
	string finalPath = filePath == "" ? prefabObject->GetName() : filePath;
	HANDLE fileHandle = FILEIO->CreateFileHandle<GameObject>(prefabObject->GetName());

	UINT32 fileVersion = FILE_VERSION_PREFAB;
	FILEIO->WriteToFile(fileHandle, fileVersion);

	vector<shared_ptr<Transform>> allObjects;
	allObjects.push_back(prefabObject->GetTransform());
	for (int i = 0; i < allObjects.size(); i++)
	{
		allObjects.insert(allObjects.end(), allObjects[i]->GetChilds().begin(), allObjects[i]->GetChilds().end());
	}
	
	FILEIO->WriteToFile(fileHandle, (UINT32)allObjects.size());
	SavePrefabRecursive(fileHandle, prefabObject, -1, prefabObject->GetName());
	CloseHandle(fileHandle);
}

void ResourceManager::SavePrefabRecursive(HANDLE fileHandle, shared_ptr<GameObject> object, int parentIdx, const string& prefabName)
{
	static int objectIdx = 0;
	if (parentIdx == -1)
		objectIdx = 0;

	int currentIdx = objectIdx++;

	FILEIO->WriteToFile(fileHandle, object->GetName());
	FILEIO->WriteToFile(fileHandle, object->GetPSOName());
	FILEIO->WriteToFile(fileHandle, object->GetTransform()->GetWorldMatrix());
	FILEIO->WriteToFile(fileHandle, parentIdx);		// 부모 인덱스
	FILEIO->WriteToFile(fileHandle, (UINT32)object->GetAllComponents().size() - 1);
	for (auto& componentVec : object->GetAllComponents()) {
		for (auto& c : componentVec) {
			if (c->type == ComponentType::Transform)
				continue;

			FILEIO->WriteToFile(fileHandle, c->type);	// 컴포넌트 타입

			switch (c->type)
			{
			case ComponentType::MeshRenderer:
			{
				auto meshRenderer = static_pointer_cast<MeshRenderer>(c);

				string meshPath = meshRenderer->GetMesh()->GetPath();	// ResourcePath
				FILEIO->WriteToFile(fileHandle, meshPath);

				string matName = UniversalUtils::ToString(meshRenderer->GetMaterial()->GetNameW());
				FILEIO->WriteToFile(fileHandle, matName);

				break;
			}

			case ComponentType::SkinnedMeshRenderer:
			{
				auto meshRenderer = static_pointer_cast<SkinnedMeshRenderer>(c);

				string meshPath = meshRenderer->GetMesh()->GetPath();	// ResourcePath
				FILEIO->WriteToFile(fileHandle, meshPath);

				string matName = UniversalUtils::ToString(meshRenderer->GetMaterial()->GetNameW());
				FILEIO->WriteToFile(fileHandle, matName);

				string rootBoneName = meshRenderer->GetRootBone()->GetGameObject()->GetName();
				FILEIO->WriteToFile(fileHandle, rootBoneName);

				string boneDataName = prefabName;
				FILEIO->WriteToFile(fileHandle, boneDataName);

				break;
			}

			case ComponentType::Animator:
			{
				auto animator = static_pointer_cast<Animator>(c);

				FILEIO->WriteToFile(fileHandle, animator->IsPlayOnInit());
				FILEIO->WriteToFile(fileHandle, animator->IsLoop());

				string currentAnimName = animator->GetCurrentAnimation()->GetName();
				FILEIO->WriteToFile(fileHandle, currentAnimName);

				auto animations = animator->GetAnimations();
				FILEIO->WriteToFile(fileHandle, (UINT32)animations.size());
				for (auto& anim : animations)
				{
					FILEIO->WriteToFile(fileHandle, anim.second->GetPath());	// ResourcePath
				}

				break;
			}

			case ComponentType::Camera:
			{
				auto camera = static_pointer_cast<Camera>(c);

				FILEIO->WriteToFile(fileHandle, camera->IsMainCamera());

				break;
			}

			case ComponentType::Script:
			{
				// 일단 씬에서 수동적으로 넣어주는 방식으로 작동중.
				// 세이브를 한다면 어떻게?
			}
			}
		}
	}

	for (auto& c : object->GetTransform()->GetChilds())
	{
		SavePrefabRecursive(fileHandle, c->GetGameObject(), currentIdx, prefabName);
	}
}

shared_ptr<Mesh> ResourceManager::LoadMesh(const string& filePath)
{
	HANDLE fileHandle = FILEIO->CreateFileHandle<Mesh>(filePath, false);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DEBUG->ErrorLog("Failed to load mesh file: " + filePath);
		return nullptr;
	}

	string meshName = filePath.substr(filePath.find_last_of("\\") + 1, filePath.length());
	meshName = meshName.substr(0, meshName.find_last_of('.'));

	// 좀 더 개선할 수 있을 것 같음. 나머지 리소스들도 다.
	if (RESOURCE->CheckResourceExists(filePath)) {
		CloseHandle(fileHandle);
		return RESOURCE->Get<Mesh>(UniversalUtils::ToWString(meshName));
	}

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

	string matName;
	FILEIO->ReadFileData(fileHandle, matName);

	CloseHandle(fileHandle);

	shared_ptr<Geometry> geometry = make_shared<Geometry>(vertices, indices);
	shared_ptr<Mesh> loadedMesh = make_shared<Mesh>(geometry);
	loadedMesh->SetName(meshName);
	loadedMesh->SetPath(filePath);
	loadedMesh->SetMaterial(RESOURCE->Get<Material>(UniversalUtils::ToWString(matName)));

	Add<Mesh>(loadedMesh->GetNameW(), loadedMesh);

	return loadedMesh;
}

shared_ptr<Animation> ResourceManager::LoadAnimation(const string& filePath)
{
	HANDLE fileHandle = FILEIO->CreateFileHandle<Animation>(filePath, false);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DEBUG->ErrorLog("Failed to load animation file: " + filePath);
		return nullptr;
	}

	string animName;
	animName = filePath.substr(filePath.find_last_of("\\") + 1, filePath.length());
	animName = animName.substr(0, animName.find_last_of('.'));

	if (RESOURCE->CheckResourceExists(filePath)) {
		CloseHandle(fileHandle);
		return RESOURCE->Get<Animation>(UniversalUtils::ToWString(animName));
	}

	float duration, ticks;
	FILEIO->ReadFileData(fileHandle, &duration, sizeof(float));
	FILEIO->ReadFileData(fileHandle, &ticks, sizeof(float));

	// filePath는 Resources/리소스타입/ 이후의 디렉토리 경로 데이터로 생각중.
	// 따라서 filePath에서 파일 이름만 추출해서 사용하도록 변경해야함.
	// 애니메이션 뿐만 아니라 모든 리소스에대해 작업 필요.
	// ㄴ해결 된건가? 기억이 안남. 검증 필요.
	shared_ptr<Animation> loadedAnimation = make_shared<Animation>(animName, duration, ticks);
	loadedAnimation->SetPath(filePath);

	UINT32 animationDataCount;
	FILEIO->ReadFileData(fileHandle, &animationDataCount, sizeof(UINT32));
	for (int i = 0; i < animationDataCount; i++)
	{
		Animation::AnimationData animationData;

		string objName;
		FILEIO->ReadFileData(fileHandle, objName);
		animationData.boneName = objName;

		UINT32 boneId;
		FILEIO->ReadFileData(fileHandle, &boneId, sizeof(UINT32));
		animationData.boneId = boneId;

		UINT32 keyFrameCount;
		FILEIO->ReadFileData(fileHandle, &keyFrameCount, sizeof(UINT32));

		for (int j = 0; j < keyFrameCount; j++)
		{
			Animation::KeyFrame keyFrame;
			FILEIO->ReadFileData(fileHandle, &keyFrame, sizeof(Animation::KeyFrame));
			animationData.keyFrames.push_back(keyFrame);
		}

		loadedAnimation->AddAnimationData(animationData);
	}

	CloseHandle(fileHandle);

	return loadedAnimation;
}

map<string, BoneData> ResourceManager::LoadBone(const string& filePath)
{
	map<string, BoneData> boneData;

	HANDLE fileHandle = FILEIO->CreateFileHandle<BoneData>(filePath, false);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DEBUG->ErrorLog("Failed to load bone file: " + filePath);
		return boneData;
	}

	UINT32 boneCount;
	FILEIO->ReadFileData(fileHandle, &boneCount, sizeof(UINT32));
	for (int i = 0; i < boneCount; i++)
	{
		BoneData bone;
		FILEIO->ReadFileData(fileHandle, bone.name);
		FILEIO->ReadFileData(fileHandle, &bone.id, sizeof(UINT));
		FILEIO->ReadFileData(fileHandle, &bone.parentId, sizeof(UINT));
		FILEIO->ReadFileData(fileHandle, &bone.offsetTransform, sizeof(XMFLOAT4X4));
		FILEIO->ReadFileData(fileHandle, &bone.localBindTransform, sizeof(XMFLOAT4X4));
		boneData[bone.name] = bone;
	}
	CloseHandle(fileHandle);

	return boneData;
}

vector<shared_ptr<GameObject>> ResourceManager::LoadPrefabObject(const string& filePath)
{
	HANDLE fileHandle = FILEIO->CreateFileHandle<GameObject>(filePath, false);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		DEBUG->ErrorLog("Failed to load prefab file: " + filePath);
		return {};
	}

	vector<shared_ptr<GameObject>> loadedObjects;
	vector<pair<int, int>> objHierarchyIdxPair;
	vector<pair<shared_ptr<SkinnedMeshRenderer>, string>> boneSettingQueue;

	UINT32 fileVersion;
	FILEIO->ReadFileData(fileHandle, &fileVersion, sizeof(UINT32));

	UINT32 objectCount;
	FILEIO->ReadFileData(fileHandle, &objectCount, sizeof(UINT32));
	for (int i = 0; i < objectCount; i++)
	{
		shared_ptr<GameObject> go = make_shared<GameObject>();
		string name, psoName;
		FILEIO->ReadFileData(fileHandle, name);
		FILEIO->ReadFileData(fileHandle, psoName);
		go->SetName(name);
		go->SetPSOName(psoName);

		XMFLOAT4X4 worldMat;
		FILEIO->ReadFileData(fileHandle, &worldMat, sizeof(XMFLOAT4X4));
		go->GetTransform()->SetWorldMatrix(worldMat);

		int parentIdx;
		FILEIO->ReadFileData(fileHandle, &parentIdx, sizeof(int));
		objHierarchyIdxPair.push_back({ i, parentIdx });

		UINT32 componentCount;
		FILEIO->ReadFileData(fileHandle, &componentCount, sizeof(UINT32));

		for (int j = 0; j < componentCount; j++)
		{
			ComponentType componentType;
			FILEIO->ReadFileData(fileHandle, &componentType, sizeof(ComponentType));

			if (fileVersion != FILE_VERSION_PREFAB)
				componentType = MapLegacyComponentTypeData(static_cast<int>(componentType), fileVersion);

			switch (componentType)
			{
				case ComponentType::MeshRenderer:
				{
					shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();

					string meshPath;
					FILEIO->ReadFileData(fileHandle, meshPath);
					meshRenderer->SetMesh(LoadMesh(meshPath));

					string matName;
					FILEIO->ReadFileData(fileHandle, matName);
					meshRenderer->SetMaterial(RESOURCE->Get<Material>(UniversalUtils::ToWString(matName)));

					go->AddComponent(meshRenderer);

					break;
				}

				case ComponentType::SkinnedMeshRenderer:
				{
					shared_ptr<SkinnedMeshRenderer> meshRenderer = make_shared<SkinnedMeshRenderer>();

					string meshPath;
					FILEIO->ReadFileData(fileHandle, meshPath);
					meshRenderer->SetMesh(LoadMesh(meshPath));

					string matName;
					FILEIO->ReadFileData(fileHandle, matName);
					meshRenderer->SetMaterial(RESOURCE->Get<Material>(UniversalUtils::ToWString(matName)));

					string rootBoneName;
					FILEIO->ReadFileData(fileHandle, rootBoneName);

					string boneDataName;
					FILEIO->ReadFileData(fileHandle, boneDataName);
					meshRenderer->SetBoneData(boneDataName);

					boneSettingQueue.push_back({ meshRenderer, rootBoneName });
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

					for (int k = 0; k < animationCount; k++)
					{
						string animationPath;
						FILEIO->ReadFileData(fileHandle, animationPath);
						animator->AddAnimation(LoadAnimation(animationPath));
					}

					animator->SetPlayOnInit(isPlayOnInit);
					animator->SetLoop(isLoop);
					animator->SetCurrentAnimation(currentAnimName);

					go->AddComponent(animator);

					break;
				}

				case ComponentType::Camera:
				{
					shared_ptr<Camera> camera = make_shared<Camera>();

					bool isMainCamera;
					FILEIO->ReadFileData(fileHandle, &isMainCamera, sizeof(bool));
					if (isMainCamera)
						camera->SetAsMainCamera();

					go->AddComponent(camera);

					break;
				}

				case ComponentType::Script:
				{

				}
			}
		}

		loadedObjects.push_back(go);
	}

	CloseHandle(fileHandle);

	for (auto& idxPair : objHierarchyIdxPair)
	{
		if (idxPair.second == -1)
			continue;

		loadedObjects[idxPair.first]->GetTransform()->SetParent(loadedObjects[idxPair.second]->GetTransform());
	}

	for (auto& renderer : boneSettingQueue)
	{
		for (auto& go : loadedObjects)
		{
			if (go->GetName() == renderer.second)
			{
				renderer.first->SetRootBone(go->GetTransform());
				break;
			}
		}
	}

	return loadedObjects;
}

ComponentType ResourceManager::MapLegacyComponentTypeData(int legacyComponentType, int fileVersion)
{
	switch (fileVersion) {
	case 1: {
		if (legacyComponentType == 1) return ComponentType::Transform;
		if (legacyComponentType == 2) return ComponentType::MeshRenderer;
		if (legacyComponentType == 3) return ComponentType::SkinnedMeshRenderer;
		if (legacyComponentType == 4) return ComponentType::Camera;
		if (legacyComponentType == 6) return ComponentType::Rigidbody;
		if (legacyComponentType == 7) return ComponentType::Light;
		if (legacyComponentType == 8) return ComponentType::Animator;
		if (legacyComponentType == 9) return ComponentType::Script;
		if (legacyComponentType == 10) return ComponentType::ParticleEmitter;
		if (legacyComponentType == 11) return ComponentType::CharacterController;
		return ComponentType::Undefined;
		break;
	}
	}
}
