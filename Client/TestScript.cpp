#include "pch.h"
#include "TestScript.h"

void TestScript::Init()
{
	assetLoader = make_shared<AssetLoader>();
	{
		assetLoader->ReadAssetFile(L"Miyu/miyu.gltf");
		RESOURCE->Add<Mesh>(L"Mesh_Miyu", assetLoader->GetLoadedMesh());
	}

	skybox = make_shared<GameObject>();
	skybox->AddComponent(make_shared<MeshRenderer>());
	skybox->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	skybox->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Mat_DefaultSkybox"));
	RENDER->AddGameObject(skybox);

	camera = make_shared<GameObject>();
	camera->AddComponent(make_shared<Camera>());
	RENDER->AddGameObject(camera);

	globalLight = make_shared<GameObject>();
	XMFLOAT4 ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	XMFLOAT4 diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
	XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	globalLight->AddComponent(make_shared<DirectionalLight>(ambient, diffuse, specular));
	RENDER->AddGameObject(globalLight);

	miyu = make_shared<GameObject>();
	miyu->meshName = "miyu";
	miyu->psoName = PSO_OPAQUE_SKINNED;
	miyu->AddComponent(make_shared<MeshRenderer>());
	miyu->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_Miyu"));
	RENDER->AddGameObject(miyu);

	sphere = make_shared<GameObject>();
	sphere->meshName = "sphere";
	sphere->AddComponent(make_shared<MeshRenderer>());
	sphere->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	RENDER->AddGameObject(sphere);

	camera->GetTransform()->SetPosition(Vector3(0.0f, 1.5f, -10.0f));
	camera->GetTransform()->LookAt(Vector3(0.0f, 1.5f, 10.0f));
	globalLight->GetTransform()->LookAt(Vector3(1.0f, -1.0f, 1.0f));
	sphere->GetTransform()->SetPosition(Vector3(3.0f, 3.0f, 3.0f));

	miyu->GetTransform()->SetScale(Vector3(1.0f, 1.0f, 1.0f));
}

void TestScript::Update()
{
	globalLight->GetTransform()->Rotate(Vector3(0.0f, 20.0f * TIME->DeltaTime(), 0.0f));
	Vector3 look = camera->GetComponent<Transform>()->GetLook();
	Vector3 right = camera->GetComponent<Transform>()->GetRight();

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
