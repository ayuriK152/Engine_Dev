#include "pch.h"
#include "TestScene.h"
#include "PlayerScript.h"
#include "EnemyScript.h"
#include "TPVCamera.h"

void TestScene::Init()
{
	{
		skybox = make_shared<GameObject>();
		skybox->SetName("skybox");
		skybox->AddComponent(make_shared<MeshRenderer>());
		skybox->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_SkyboxSphere"));
		skybox->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Mat_DefaultSkybox"));
		skybox->SetPSOName(PSO_SKYBOX);
		gameObjects.push_back(skybox);
		RENDER->SetSkyboxTexture(RESOURCE->Get<Texture>(L"Tex_DefaultSkybox"));
	}

	{
		camera = make_shared<GameObject>();
		camera->SetName("camera");
		camera->AddComponent(make_shared<Camera>());
		gameObjects.push_back(camera);
	}

	{
		globalLight = make_shared<GameObject>();
		globalLight->SetName("GlobalLight");
		XMFLOAT4 ambient = { 0.8f, 0.8f, 0.8f, 1.0f };
		XMFLOAT4 diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
		XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
		globalLight->AddComponent(make_shared<DirectionalLight>(ambient, diffuse, specular));
		globalLight->GetTransform()->LookAt(Vector3(1.0f, -2.0f, 1.0f));
		gameObjects.push_back(globalLight);
	}

	{
		auto loadedObjects = RESOURCE->LoadPrefabObject("Paladin WProp J Nordstrom");
		player = loadedObjects[0];
		gameObjects.insert(gameObjects.end(), loadedObjects.begin(), loadedObjects.end());
		player->GetComponent<Animator>()->SetBones(RESOURCE->LoadBone("Paladin WProp J Nordstrom"));
		player->AddComponent(make_shared<PlayerScript>());
	}

	{
		test = make_shared<GameObject>();
		test->SetName("test");
		test->AddComponent(make_shared<MeshRenderer>());
		test->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->LoadMesh("Dungeon assets\\StoneWall1"));
		test->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Stone wall"));
		test->GetTransform()->SetPosition({ 0, 1, 0 });
		test->GetTransform()->SetRotation({ -90, 0, 0 });
		
		gameObjects.push_back(test);
	}

	{
		tpvCameraArm = make_shared<GameObject>();
		tpvCameraArm->SetName("TPVCameraArm");
		tpvCameraArm->AddComponent(make_shared<TPVCamera>());
		auto tpvCameraScript = static_pointer_cast<TPVCamera>(tpvCameraArm->GetComponent<Script>());
		tpvCameraScript->cameraTransform = camera->GetTransform();
		tpvCameraScript->targetTransform = player->GetTransform();
		tpvCameraScript->offset = Vector3(0.0f, 1.5f, 0.0f);
		tpvCameraScript->isCameraControllOn = false;
		gameObjects.push_back(tpvCameraArm);
	}

	{
		box = make_shared<GameObject>();
		box->SetName("box");
		box->AddComponent(make_shared<MeshRenderer>());
		box->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Mesh_BasicBox"));
		box->GetComponent<MeshRenderer>()->GetMaterial()->SetDiffuse(RESOURCE->Get<Texture>(L"0dot001mm_1"));
		box->AddComponent(make_shared<BoxCollider>());
		box->GetTransform()->SetPosition(Vector3(5.0f, 0.0f, 5.0f));
		box->GetTransform()->SetScale(Vector3(5.0f, 1.0f, 5.0f));
		box->GetTransform()->SetRotation(Vector3(0.0f, 0.0f, 20.0f));
		gameObjects.push_back(box);
	}

	{
		ground = make_shared<GameObject>();
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
	}

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
	if (INPUTM->IsKeyDown(KeyValue::ESC))
		GAMEAPP->ExitApplication();

	if (INPUTM->IsKeyPress(KeyValue::V))
		RENDER->SetCurrPSO(PSO_WIREFRAME);

	if (INPUTM->IsKeyDown(KeyValue::NUM_1))
	{
		INPUTM->SetMouseCenterFixMode(!INPUTM->IsMouseCenterFixed());
		auto tpvCameraScript = static_pointer_cast<TPVCamera>(tpvCameraArm->GetComponent<Script>());
		tpvCameraScript->isCameraControllOn = !tpvCameraScript->isCameraControllOn;
	}
	else
		RENDER->SetDefaultPSO();
}
