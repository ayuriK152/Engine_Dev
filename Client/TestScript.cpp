#include "pch.h"
#include "TestScript.h"

void TestScript::Init()
{
	assetLoader = make_shared<AssetLoader>();
	{
		assetLoader->ReadAssetFile(L"Y Bot\\Y Bot.fbx");
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
	//model->psoName = PSO_OPAQUE_SKINNED;
	gameObjects.push_back(model);

	sphere = make_shared<GameObject>();
	sphere->name = "sphere";
	sphere->AddComponent(make_shared<MeshRenderer>());
	sphere->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	gameObjects.push_back(sphere);

	sphere2 = make_shared<GameObject>();
	sphere2->name = "sphere2";
	sphere2->AddComponent(make_shared<MeshRenderer>());
	sphere2->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	gameObjects.push_back(sphere2);

	camera->GetTransform()->SetPosition(Vector3(0.0f, 1.5f, -10.0f));
	camera->GetTransform()->LookAt(Vector3(0.0f, 1.5f, 10.0f));
	globalLight->GetTransform()->LookAt(Vector3(1.0f, -1.0f, 1.0f));
	sphere->GetTransform()->SetPosition(Vector3(3.0f, 1.5f, 3.0f));
	sphere2->GetTransform()->SetScale(Vector3(0.5f, 0.5f, 0.5f));
	sphere2->GetTransform()->SetParent(sphere->GetTransform());
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

	sphere->GetTransform()->Rotate(Vector3(0.0f, 40.0f * TIME->DeltaTime(), 0.0f));
	//sphere2->GetTransform()->Translate(MathHelper::VectorMultiply(look, TIME->DeltaTime() * 5.0f));

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
