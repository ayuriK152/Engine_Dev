#include "pch.h"
#include "PhysicsTestScene.h"

void PhysicsTestScene::Init()
{
	auto skybox = make_shared<GameObject>();
	skybox->SetName("skybox");
	skybox->AddComponent(make_shared<MeshRenderer>());
	skybox->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_SkyboxSphere"));
	skybox->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Mat_DefaultSkybox"));
	skybox->SetPSOName(PSO_SKYBOX);
	gameObjects.push_back(skybox);
	RENDER->SetSkyboxTexture(RESOURCE->Get<Texture>(L"Tex_DefaultSkybox"));

	auto camera = make_shared<GameObject>();
	camera->SetName("camera");
	camera->AddComponent(make_shared<Camera>());
	gameObjects.push_back(camera);

	auto globalLight = make_shared<GameObject>();
	globalLight->SetName("GlobalLight");
	XMFLOAT4 ambient = { 0.8f, 0.8f, 0.8f, 1.0f };
	XMFLOAT4 diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	globalLight->AddComponent(make_shared<DirectionalLight>(ambient, diffuse, specular));
	globalLight->GetTransform()->LookAt(Vector3(1.0f, -2.0f, 1.0f));
	gameObjects.push_back(globalLight);

	auto ground = make_shared<GameObject>();
	ground->SetName("ground");
	ground->AddComponent(make_shared<MeshRenderer>());
	ground->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
	auto mat = make_shared<Material>("Proto_dark1", L"prototype_dark1");
	mat->tilling = { 100.0f, 100.0f };
	RESOURCE->Add<Material>(L"Mat_Proto_dark1", mat);
	ground->GetComponent<MeshRenderer>()->SetMaterial(mat);
	ground->AddComponent(make_shared<BoxCollider>());
	ground->GetTransform()->SetPosition(Vector3(0.0f, -0.5f, 0.0f));
	ground->GetTransform()->SetScale(Vector3(100.0f, 1.0f, 100.0f));
	ground->GetTransform()->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
	gameObjects.push_back(ground);

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

void PhysicsTestScene::Update()
{

}
