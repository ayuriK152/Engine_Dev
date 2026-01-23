#include "pch.h"
#include "AnimationTestScene.h"
#include "EditorCamera.h"

void AnimationTestScene::Init()
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
	camera->AddComponent(make_shared<EditorCamera>());
	camera->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, -10.0f));
	gameObjects.push_back(camera);

	auto loadedObjects = RESOURCE->LoadPrefabObject("Paladin WProp J Nordstrom");
	loadedObjects[0]->GetComponent<Animator>()->SetBone("Paladin WProp J Nordstrom");
	loadedObjects[0]->GetComponent<Animator>()->SetCurrentAnimation("idle_sword_4");
	loadedObjects[0]->GetComponent<Animator>()->SetLoop(true);
	loadedObjects[0]->GetTransform()->SetPosition(Vector3(-5.0f, 0.0f, 0.0f));
	gameObjects.insert(gameObjects.end(), loadedObjects.begin(), loadedObjects.end());

	auto loadedObjects2 = RESOURCE->LoadPrefabObject("Paladin WProp J Nordstrom");
	loadedObjects2[0]->GetComponent<Animator>()->SetBone("Paladin WProp J Nordstrom");
	loadedObjects2[0]->GetComponent<Animator>()->SetCurrentAnimation("idle_sword_4");
	loadedObjects2[0]->GetComponent<Animator>()->SetLoop(true);
	loadedObjects2[0]->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	gameObjects.insert(gameObjects.end(), loadedObjects2.begin(), loadedObjects2.end());

	auto loadedObjects3 = RESOURCE->LoadPrefabObject("Paladin WProp J Nordstrom");
	loadedObjects3[0]->GetComponent<Animator>()->SetBone("Paladin WProp J Nordstrom");
	loadedObjects3[0]->GetComponent<Animator>()->SetCurrentAnimation("idle_sword_4");
	loadedObjects3[0]->GetComponent<Animator>()->SetLoop(true);
	loadedObjects3[0]->GetTransform()->SetPosition(Vector3(5.0f, 0.0f, 0.0f));
	gameObjects.insert(gameObjects.end(), loadedObjects3.begin(), loadedObjects3.end());

	auto globalLight = make_shared<GameObject>();
	globalLight->SetName("GlobalLight");
	XMFLOAT4 ambient = { 0.8f, 0.8f, 0.8f, 1.0f };
	XMFLOAT4 diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	globalLight->AddComponent(make_shared<DirectionalLight>(ambient, diffuse, specular));
	globalLight->GetTransform()->LookAt(Vector3(1.0f, -2.0f, 1.0f));
	gameObjects.push_back(globalLight);

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

void AnimationTestScene::Update()
{

}
