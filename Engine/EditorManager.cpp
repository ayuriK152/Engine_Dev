#include "pch.h"
#include "EditorManager.h"

EditorManager::~EditorManager()
{

}

void EditorManager::Init()
{
	shared_ptr<GameObject> cameraObj = GameObject::Instantiate();
	cameraObj->SetName("EditorCamera");
	cameraObj->SetTag("EditorCamera");
	cameraObj->GetTransform()->SetPosition({ 0, 3, 0 });
	cameraObj->AddComponent(ComponentFactory::Create("Camera"));
	cameraObj->AddComponent(ComponentFactory::Create("EditorCamera"));
}

void EditorManager::Play()
{
	if (Camera::GetCurrentCamera() == nullptr) {
		DEBUG->ErrorLog("Main camera does not exists! Can not start this scene!");
		return;
	}

	auto objects = RENDER->GetObjects();
	
	for (auto& go : objects) {
		GameObjectSnapshot goSnapshot = go->CaptureSnapshot();

		auto comps = go->GetAllComponents();
		for (auto& compVec : comps) {
			for (auto& c : compVec) {
				goSnapshot.compSnapshotIndices.push_back(_compSnapshots.size());
				_compSnapshots.push_back(c->CaptureSnapshot());
			}
		}

		_objectSnapshots.push_back(goSnapshot);
	}

	_isOnPlay = true;
}

void EditorManager::Stop()
{
	auto& objects = RENDER->GetObjects();

	// 런타임중 생성된 오브젝트 삭제
	for (int i = 0; i < objects.size(); ++i) {
		if (!objects[i]->IsSnapshotCaptured()) {
			RENDER->DeleteGameobject(objects[i]);
			--i;
		}
	}

	// 삭제되지 않은 오브젝트들 복구
	for (auto& go : objects) {
		for (int i = 0; i < _objectSnapshots.size(); ++i) {
			GameObjectSnapshot objectSnapshot = _objectSnapshots[i];

			if (go->GetId() == objectSnapshot.id) {
				go->RestoreSnapshot(objectSnapshot);

				RestoreObjectComponents(go, objectSnapshot);

				_objectSnapshots.erase(_objectSnapshots.begin() + i);
				--i;
			}
		}
	}

	// 삭제된 오브젝트들 복구
	for (auto& objectSnapshot : _objectSnapshots) {
		shared_ptr<GameObject> go = GameObject::Instantiate();
		go->RestoreSnapshot(objectSnapshot);

		RestoreObjectComponents(go, objectSnapshot);
	}

	_objectSnapshots.clear();
	_compSnapshots.clear();

	_isOnPlay = false;
}

void EditorManager::StoreState()
{

}

void EditorManager::RestoreState()
{

}

void EditorManager::RestoreObjectComponents(shared_ptr<GameObject> go, GameObjectSnapshot objectSnapshot)
{
	auto comps = go->GetAllComponents();
	for (int compIdx : objectSnapshot.compSnapshotIndices) {
		ComponentSnapshot compSnapshot = _compSnapshots[compIdx];

		bool flag = false;
		for (auto& compVec : comps) {
			for (auto& c : compVec) {
				if (c->GetID() == compSnapshot.id) {
					c->RestoreSnapshot(compSnapshot);
					flag = true;
					break;
				}
			}
			if (flag) break;
		}
		// 런타임 중에 삭제된 경우
		if (!flag) {
			shared_ptr<Component> comp = ComponentFactory::Create(compSnapshot.componentType);
			go->AddComponent(comp);
			comp->RestoreSnapshot(compSnapshot);
		}
	}
}
