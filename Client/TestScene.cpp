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

		shared_ptr<Texture> proto1 = make_shared<Texture>(L"prototype_dark1.dds");
		RESOURCE->Add<Texture>(L"proto_dark1", proto1);
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
		//auto loadedObjects = RESOURCE->LoadPrefabObject("Y Bot");
		//model = loadedObjects[0];
		//gameObjects.insert(gameObjects.end(), loadedObjects.begin(), loadedObjects.end());
		//model->AddComponent(make_shared<PlayerScript>());
	}

	{
		box = make_shared<GameObject>();
		box->name = "box";
		box->AddComponent(make_shared<MeshRenderer>());
		box->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
		box->GetComponent<MeshRenderer>()->GetMaterial()->SetTexture(RESOURCE->Get<Texture>(L"0dot001mm_1"));
		box->AddComponent(make_shared<BoxCollider>());
		box->AddComponent(make_shared<Rigidbody>());
		box->GetComponent<Rigidbody>()->isGravity = false;
		box->GetTransform()->SetPosition(Vector3(0.0f, 5.0f, 0.0f));
		box->GetTransform()->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
		gameObjects.push_back(box);
	}

	{
		ground = make_shared<GameObject>();
		ground->name = "ground";
		ground->AddComponent(make_shared<MeshRenderer>());
		ground->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
		auto mat = make_shared<Material>("Proto_dark1", L"proto_dark1");
		mat->tilling = { 100.0f, 100.0f };
		RESOURCE->Add<Material>(L"Mat_Proto_dark1", mat);
		ground->GetComponent<MeshRenderer>()->SetMaterial(mat);
		ground->AddComponent(make_shared<BoxCollider>());
		ground->GetTransform()->SetPosition(Vector3(0.0f, -0.5f, 0.0f));
		ground->GetTransform()->SetScale(Vector3(100.0f, 1.0f, 100.0f));
		ground->GetTransform()->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
		gameObjects.push_back(ground);
	}

	{
		//box2 = make_shared<GameObject>();
		//box2->name = "box";
		//box2->AddComponent(make_shared<MeshRenderer>());
		//box2->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
		//box2->AddComponent(make_shared<BoxCollider>());
		////box2->AddComponent(make_shared<Rigidbody>());
		//box2->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
		//box2->GetTransform()->SetRotation(Vector3(0.0f, 0.0f, -45.0f));
		//gameObjects.push_back(box2);
	}

	//{
	//	sphere = make_shared<GameObject>();
	//	sphere->name = "sphere";
	//	sphere->AddComponent(make_shared<MeshRenderer>());
	//	sphere->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	//	sphere->GetComponent<MeshRenderer>()->GetMaterial()->SetTexture(RESOURCE->Get<Texture>(L"0dot001mm_1"));	// 텍스쳐 테스트용 나중에 삭제
	//	sphere->AddComponent(make_shared<SphereCollider>());
	//	sphere->AddComponent(make_shared<SphereScript>());
	//	sphere->GetTransform()->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
	//	gameObjects.push_back(sphere);
	//}

	//{
	//	sphere2 = make_shared<GameObject>();
	//	sphere2->name = "sphere2";
	//	sphere2->AddComponent(make_shared<MeshRenderer>());
	//	sphere2->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	//	sphere2->GetComponent<MeshRenderer>()->GetMaterial()->SetTexture(RESOURCE->Get<Texture>(L"0dot001mm_1"));	// 텍스쳐 테스트용 나중에 삭제
	//	sphere2->AddComponent(make_shared<SphereCollider>());
	//	sphere2->AddComponent(make_shared<SphereScript>());
	//	sphere2->AddComponent(make_shared<Rigidbody>());
	//	sphere2->GetTransform()->SetPosition(Vector3(2.0f, 4.0f, 0.0f));
	//	gameObjects.push_back(sphere2);
	//}

	camera->GetTransform()->SetPosition(Vector3(0.0f, 1.5f, -10.0f));
	camera->GetTransform()->LookAt(Vector3(0.0f, 1.5f, 10.0f));

	globalLight->GetTransform()->LookAt(Vector3(1.0f, -1.0f, 1.0f));

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
	//box->GetTransform()->Rotate(Vector3(0.0f, 1.0f * TIME->DeltaTime(), 1.0f * TIME->DeltaTime()));
	if (INPUTM->IsKeyPress(KeyValue::ESC))
		GAMEAPP->ExitApplication();

	if (INPUTM->IsKeyDown(KeyValue::A))
		box->GetComponent<Rigidbody>()->AddTorque(Vector3(0.0f, 0.0f, 1.0f));
	if (INPUTM->IsKeyPress(KeyValue::V))
		RENDER->SetCurrPSO(PSO_WIREFRAME);
	else
		RENDER->SetDefaultPSO();
}
