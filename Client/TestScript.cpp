#include "pch.h"
#include "TestScript.h"

void TestScript::Init()
{
	assetLoader = make_shared<AssetLoader>();
	{
		assetLoader->ImportAssetFile(L"Y Bot\\Y Bot.fbx");
	}

	// 텍스쳐 테스트용 나중에 삭제
	{
		shared_ptr<Texture> koyuki = make_shared<Texture>(L"0dot001mm_1.dds");
		RESOURCE->Add<Texture>(L"0dot001mm_1", koyuki);
	}

	//file io test
	{
		HANDLE testHandle = FILEIO->CreateFileHandle<Mesh>("test");
		int test = 1;
		int test2 = 15;
		string test3 = "abcd";
		FILEIO->WriteToFile(testHandle, test);
		FILEIO->WriteToFile(testHandle, test2);
		FILEIO->WriteToFile(testHandle, test3);
		CloseHandle(testHandle);

		testHandle = FILEIO->CreateFileHandle<Mesh>("test");
		int a;
		FILEIO->ReadFileData(testHandle, a);
		int b;
		FILEIO->ReadFileData(testHandle, b);
		string c;
		FILEIO->ReadFileData(testHandle, c);

		CloseHandle(testHandle);
	}

	skybox = make_shared<GameObject>();
	skybox->name = "skybox";
	skybox->AddComponent(make_shared<MeshRenderer>());
	skybox->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	skybox->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Mat_DefaultSkybox"));
	skybox->psoName = PSO_SKYBOX;
	gameObjects.push_back(skybox);
	RENDER->SetSkyboxTexture(RESOURCE->Get<Texture>(L"Tex_DefaultSkybox"));

	camera = make_shared<GameObject>();
	camera->name = "camera";
	camera->AddComponent(make_shared<Camera>());
	gameObjects.push_back(camera);

	globalLight = make_shared<GameObject>();
	XMFLOAT4 ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	XMFLOAT4 diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
	XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	globalLight->AddComponent(make_shared<DirectionalLight>(ambient, diffuse, specular));
	gameObjects.push_back(globalLight);

	model = assetLoader->GetLoadedObject();
	model->name = "model";
	model->SetPSOName(PSO_OPAQUE_SKINNED);
	gameObjects.push_back(model);

	sphere = make_shared<GameObject>();
	sphere->name = "sphere";
	sphere->AddComponent(make_shared<MeshRenderer>());
	sphere->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	sphere->GetComponent<MeshRenderer>()->GetMaterial()->SetTexture(RESOURCE->Get<Texture>(L"0dot001mm_1"));	// 텍스쳐 테스트용 나중에 삭제
	gameObjects.push_back(sphere);

	sphere2 = make_shared<GameObject>();
	sphere2->name = "sphere2";
	sphere2->AddComponent(make_shared<MeshRenderer>());
	sphere2->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	gameObjects.push_back(sphere2);

	sphere3 = make_shared<GameObject>();
	sphere3->name = "sphere3";
	sphere3->AddComponent(make_shared<MeshRenderer>());
	sphere3->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	gameObjects.push_back(sphere3);

	camera->GetTransform()->SetPosition(Vector3(0.0f, 1.5f, -10.0f));
	camera->GetTransform()->LookAt(Vector3(0.0f, 1.5f, 10.0f));
	globalLight->GetTransform()->LookAt(Vector3(1.0f, -1.0f, 1.0f));

	{
		sphere->GetTransform()->SetPosition(Vector3(0.0f, 1.5f, 0.0f));
		sphere2->GetTransform()->SetPosition(Vector3(2.5f, 1.5f, 0.0f));
		sphere2->GetTransform()->SetScale(Vector3(0.5f, 0.5f, 0.5f));
		sphere2->GetTransform()->SetParent(sphere->GetTransform());
		sphere3->GetTransform()->SetPosition(Vector3(3.0f, 1.5f, 0.0f));
		sphere3->GetTransform()->SetScale(Vector3(0.2f, 0.2f, 0.2f));
		sphere3->GetTransform()->SetParent(sphere2->GetTransform());

		sphere->GetTransform()->SetPosition(Vector3(5.0f, 1.0f, 0.0f));
	}

	model->GetTransform()->SetScale(Vector3(0.1f, 0.1f, 0.1f));

	
	while (gameObjects.size() > 0)
	{
		RENDER->AddGameObject(gameObjects[0]);
		for (auto& t : gameObjects[0]->GetTransform()->GetChilds())
		{
			gameObjects.push_back(t->GetGameObject());
		}
		gameObjects.erase(gameObjects.begin());
	}
}

void TestScript::Update()
{
	Vector3 look = camera->GetComponent<Transform>()->GetLook();
	Vector3 right = camera->GetComponent<Transform>()->GetRight();

	sphere->GetTransform()->Rotate(Vector3(0.0f, 30.0f * TIME->DeltaTime(), 0.0f));
	sphere2->GetTransform()->Rotate(Vector3(0.0f, 0.0f, 70.0f * TIME->DeltaTime()));

	if (INPUTM->IsKeyPress(KeyValue::W))
		camera->GetTransform()->Translate(MathHelper::VectorMultiply(look, TIME->DeltaTime() * 5.0f));
	if (INPUTM->IsKeyPress(KeyValue::S))
		camera->GetTransform()->Translate(MathHelper::VectorMultiply(look, -TIME->DeltaTime() * 5.0f));
	if (INPUTM->IsKeyPress(KeyValue::A))
		camera->GetTransform()->Translate(MathHelper::VectorMultiply(right, TIME->DeltaTime() * 5.0f));
	if (INPUTM->IsKeyPress(KeyValue::D))
		camera->GetTransform()->Translate(MathHelper::VectorMultiply(right, -TIME->DeltaTime() * 5.0f));
	
	if (INPUTM->IsKeyPress(KeyValue::Q))
		camera->GetTransform()->Rotate(Vector3(0.0f, -20.0f * TIME->DeltaTime(), 0.0f));
	if (INPUTM->IsKeyPress(KeyValue::E))
		camera->GetTransform()->Rotate(Vector3(0.0f, 20.0f * TIME->DeltaTime(), 0.0f));


	if (INPUTM->IsKeyPress(KeyValue::ESC))
		GAMEAPP->ExitApplication();

	if (INPUTM->IsKeyPress(KeyValue::V))
		RENDER->SetCurrPSO(PSO_WIREFRAME);
	else
		RENDER->SetDefaultPSO();
}
