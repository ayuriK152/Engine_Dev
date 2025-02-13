#include "pch.h"
#include "TestScript.h"

void TestScript::Init()
{
	auto camera = make_shared <GameObject>();
	camera->AddComponent(make_shared<Camera>());
	auto cameraInstance = RENDER->AddGameObject(camera);

	auto box = make_shared<GameObject>();
	box->meshName = "box";
	box->AddComponent(make_shared<MeshRenderer>());
	box->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
	box->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Mat_Default"));
	auto boxInstance = RENDER->AddGameObject(box);

	auto sphere = make_shared<GameObject>();
	sphere->meshName = "sphere";
	sphere->AddComponent(make_shared<MeshRenderer>());
	sphere->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	sphere->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Mat_Default"));
	auto sphereInstance = RENDER->AddGameObject(sphere);

	camera->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, -10.0f));
	camera->GetTransform()->SetRotation(Vector3(0.0f, -180.0f, 0.0f));
	camera->GetTransform()->LookAt(Vector3(0.0f, 0.0f, 10.0f));
	boxInstance->GetTransform()->SetPosition(Vector3(-3.0f, 0.0f, 10.0f));
	sphereInstance->GetTransform()->SetPosition(Vector3(3.0f, 0.0f, 10.0f));
}

void TestScript::Update()
{

}
