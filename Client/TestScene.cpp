#include "pch.h"
#include "TestScene.h"
#include "PlayerScript.h"
#include "SphereScript.h"

void TestScene::Init()
{
	// 텍스쳐 테스트용 나중에 삭제
	{
		shared_ptr<Texture> koyuki = make_shared<Texture>(L"0dot001mm_1.dds");
		RESOURCE->Add<Texture>(L"0dot001mm_1", koyuki);
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

	{
		auto loadedObjects = RESOURCE->LoadPrefabObject("Y Bot");
		model = loadedObjects[0];
		gameObjects.insert(gameObjects.end(), loadedObjects.begin(), loadedObjects.end());
		model->AddComponent(make_shared<PlayerScript>());
	}

	{
		sphere = make_shared<GameObject>();
		sphere->name = "sphere";
		sphere->AddComponent(make_shared<MeshRenderer>());
		sphere->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
		sphere->GetComponent<MeshRenderer>()->GetMaterial()->SetTexture(RESOURCE->Get<Texture>(L"0dot001mm_1"));	// 텍스쳐 테스트용 나중에 삭제
		sphere->AddComponent(make_shared<SphereScript>());
		gameObjects.push_back(sphere);
	}

	{
		cube1 = make_shared<GameObject>();
		cube1->name = "cube1";
		cube1->AddComponent(make_shared<MeshRenderer>());
		cube1->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
		cube1->AddComponent(make_shared<BoxCollider>());
		gameObjects.push_back(cube1);
	}

	{
		cube2 = make_shared<GameObject>();
		cube2->name = "cube2";
		cube2->AddComponent(make_shared<MeshRenderer>());
		cube2->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
		cube2->AddComponent(make_shared<BoxCollider>());
		cube2->GetTransform()->SetPosition(Vector3(-3.0f, 0.0f, 0.0f));
		gameObjects.push_back(cube2);
	}

	camera->GetTransform()->SetPosition(Vector3(0.0f, 1.5f, -10.0f));
	camera->GetTransform()->LookAt(Vector3(0.0f, 1.5f, 10.0f));

	globalLight->GetTransform()->LookAt(Vector3(1.0f, -1.0f, 1.0f));

	sphere->GetTransform()->SetPosition(Vector3(0.0f, 1.5f, 0.0f));
	sphere->GetTransform()->SetPosition(Vector3(3.0f, 1.0f, 0.0f));

	
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

void TestScene::Update()
{
	if (cube2->GetComponent<Collider>()->IsOnColliding())
	{
		cout << "colliding" << endl;
	}

	if (INPUTM->IsKeyPress(KeyValue::F))
	{
		cube2->GetTransform()->Translate(Vector3(1.0f * TIME->DeltaTime(), 0.0f, 0.0f));
	}

	if (INPUTM->IsKeyPress(KeyValue::ESC))
		GAMEAPP->ExitApplication();

	if (INPUTM->IsKeyPress(KeyValue::V))
		RENDER->SetCurrPSO(PSO_WIREFRAME);
	else
		RENDER->SetDefaultPSO();
}
