#include "pch.h"
#include "DungeonScene.h"
#include "PlayerScript.h"
#include "TPVCamera.h"

void DungeonScene::Init()
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
		player->AddComponent(make_shared<PlayerScript>());
	}

	{
		auto loadedObjects = RESOURCE->LoadPrefabObject("Brute");
		auto boss = loadedObjects[0];
		gameObjects.insert(gameObjects.end(), loadedObjects.begin(), loadedObjects.end());
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
		RESOURCE->LoadMesh("Dungeon assets\\Tiles4x4");
		for (int z = -2; z <= 2; z++) {
			for (int x = -2; x <= 2; x++) {
				auto floorTile = make_shared<GameObject>();
				floorTile->SetName("FloorTiles");
				floorTile->AddComponent(make_shared<MeshRenderer>());
				floorTile->GetComponent<MeshRenderer>()->SetMesh(RESOURCE->Get<Mesh>(L"Tiles4x4"));
				floorTile->GetComponent<MeshRenderer>()->SetMaterial(RESOURCE->Get<Material>(L"Stone wall"));
				floorTile->AddComponent(make_shared<BoxCollider>());
				floorTile->GetTransform()->SetPosition({ 4.0f * x, 0, 4.0f * z });
				floorTile->GetTransform()->SetRotation({ -90, 0, 0 });

				gameObjects.push_back(floorTile);
			}
		}
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

void DungeonScene::Update()
{
	if (INPUTM->IsKeyDown(KeyValue::NUM_1))
	{
		INPUTM->SetMouseCenterFixMode(!INPUTM->IsMouseCenterFixed());
		auto tpvCameraScript = static_pointer_cast<TPVCamera>(tpvCameraArm->GetComponent<Script>());
		tpvCameraScript->isCameraControllOn = !tpvCameraScript->isCameraControllOn;
	}
}
