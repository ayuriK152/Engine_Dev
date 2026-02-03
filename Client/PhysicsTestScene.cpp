#include "pch.h"
#include "PhysicsTestScene.h"
#include "EditorCamera.h"
#include "PlayerScript.h"
#include "EnemyScript.h"

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
	camera->AddComponent(make_shared<EditorCamera>());
	camera->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, -10.0f));
	gameObjects.push_back(camera);

	auto loadedObjects = RESOURCE->LoadPrefabObject("Paladin WProp J Nordstrom");
	loadedObjects[0]->GetComponent<Animator>()->SetBone("Paladin WProp J Nordstrom");
	loadedObjects[0]->AddComponent(make_shared<PlayerScript>());
	gameObjects.insert(gameObjects.end(), loadedObjects.begin(), loadedObjects.end());

	auto brute = RESOURCE->LoadPrefabObject("Brute");
	brute[0]->GetComponent<Animator>()->SetBone("Brute");
	shared_ptr<EnemyScript> enemyScript = make_shared<EnemyScript>();
	enemyScript->target = loadedObjects[0];
	brute[0]->AddComponent(enemyScript);
	gameObjects.insert(gameObjects.end(), brute.begin(), brute.end());

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
	ground->GetTransform()->SetPosition(Vector3(0.0f, -0.5f, 0.0f));
	ground->GetTransform()->SetScale(Vector3(100.0f, 1.0f, 100.0f));
	ground->GetTransform()->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
	ground->AddComponent(make_shared<Rigidbody>());
	ground->GetComponent<Rigidbody>()->isGravity = false;
	gameObjects.push_back(ground);

	auto cube1 = make_shared<GameObject>();
	cube1->SetName("Cube1");
	cube1->AddComponent(make_shared<Rigidbody>());
	cube1->AddComponent(make_shared<MeshRenderer>());
	cube1->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
	cube1->GetComponent<MeshRenderer>()->GetMaterial()->SetDiffuse(RESOURCE->Get<Texture>(L"0dot001mm_1"));
	cube1->GetTransform()->SetPosition(Vector3(3.0f, 5.0f, 3.0f));
	gameObjects.push_back(cube1);

	auto sphere1 = make_shared<GameObject>();
	sphere1->SetName("Sphere1");
	sphere1->AddComponent(make_shared<Rigidbody>());
	sphere1->GetComponent<Rigidbody>()->SetColliderShape(ColliderShape::Shpere);
	sphere1->AddComponent(make_shared<MeshRenderer>());
	sphere1->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicSphere"));
	sphere1->GetComponent<MeshRenderer>()->GetMaterial()->SetDiffuse(RESOURCE->Get<Texture>(L"0dot001mm_1"));
	sphere1->GetTransform()->SetPosition(Vector3(-3.0f, 5.0f, 3.0f));
	gameObjects.push_back(sphere1);

	//auto particleObj = make_shared<GameObject>();
	//particleObj->SetName("ParticleObj");
	//auto particleEmitter = make_shared<ParticleEmitter>();
	//particleEmitter->SetPlay(true);
	//particleEmitter->SetParticleTexture("ParticleTest.png");
	//particleObj->AddComponent(particleEmitter);
	//particleObj->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
	//gameObjects.push_back(particleObj);

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
