#include "pch.h"
#include "TestScript.h"

void TestScript::Init()
{
	assetLoader = make_shared<AssetLoader>();
	{
		assetLoader->ReadAssetFile(L"Miyu/miyu.fbx");
		RESOURCE->Add<Mesh>(L"Mesh_Miyu", assetLoader->GetLoadedMesh());
		//RESOURCE->Get<Material>(L"Miyu_Head")->SetTexture();
	}

	camera = make_shared <GameObject>();
	camera->AddComponent(make_shared<Camera>());
	RENDER->AddGameObject(camera);

	miyu = make_shared<GameObject>();
	miyu->meshName = "miyu";
	miyu->AddComponent(make_shared<MeshRenderer>());
	miyu->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_Miyu"));
	RENDER->AddGameObject(miyu);

	box = make_shared<GameObject>();
	box->meshName = "box";
	box->AddComponent(make_shared<MeshRenderer>());
	//box->AddComponent(make_shared<Rigidbody>());
	//box->AddComponent(make_shared<BoxCollider>());
	box->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
	RENDER->AddGameObject(box);

	auto sphere = make_shared<GameObject>();
	sphere->meshName = "sphere";
	sphere->AddComponent(make_shared<MeshRenderer>());
	sphere->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	RENDER->AddGameObject(sphere);

	auto quad = make_shared<GameObject>();
	quad->meshName = "quad";
	quad->AddComponent(make_shared<MeshRenderer>());
	//quad->AddComponent(make_shared<BoxCollider>());
	quad->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicQuad"));
	RENDER->AddGameObject(quad);

	camera->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, -10.0f));
	camera->GetTransform()->LookAt(Vector3(0.0f, 0.0f, 10.0f));
	box->GetTransform()->SetPosition(Vector3(0.0f, 5.0f, 5.0f));
	box->GetTransform()->SetRotation(Vector3(45.0f, 0.0f, 0.0f));
	sphere->GetTransform()->SetPosition(Vector3(3.0f, 0.0f, 10.0f));
	quad->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, 5.0f));
	quad->GetTransform()->SetRotation(Vector3(90.0f, 0.0f, 0.0f));
	quad->GetTransform()->SetScale(Vector3(1.0f, 1.0f, 1.0f));

	miyu->GetTransform()->SetScale(Vector3(0.02f, 0.02f, 0.02f));

	//box->GetComponent<Rigidbody>()->isGravity = true;
}

void TestScript::Update()
{
	//camera->GetTransform()->LookAt(box->GetTransform()->GetPosition());
	Vector3 look = camera->GetComponent<Transform>()->GetLook();
	Vector3 right = camera->GetComponent<Transform>()->GetRight();
	if (INPUTM->IsKeyPress(KeyValue::W))
		camera->GetComponent<Transform>()->Translate(MathHelper::VectorMultiply(look, TIME->DeltaTime() * 5.0f));
	if (INPUTM->IsKeyPress(KeyValue::S))
		camera->GetComponent<Transform>()->Translate(MathHelper::VectorMultiply(look, -TIME->DeltaTime() * 5.0f));
	if (INPUTM->IsKeyPress(KeyValue::A))
		camera->GetComponent<Transform>()->Translate(MathHelper::VectorMultiply(right, TIME->DeltaTime() * 5.0f));
	if (INPUTM->IsKeyPress(KeyValue::D))
		camera->GetComponent<Transform>()->Translate(MathHelper::VectorMultiply(right, -TIME->DeltaTime() * 5.0f));
	
	if (INPUTM->IsKeyPress(KeyValue::ESC))
		GAMEAPP->ExitApplication();

	//if (INPUTM->IsKeyDown(KeyValue::V))
	//	box->GetComponent<Rigidbody>()->AddForce(Vector3(0.0f, 5.0f, 0.0f));

	if (INPUTM->IsKeyPress(KeyValue::V))
		RENDER->SetCurrPSO(WIREFRAME);
	else
		RENDER->SetCurrPSO(OPAQUE_SOLID);
}
