#include "pch.h"
#include "GameObject.h"

int GameObject::_idCount = 0;

GameObject::GameObject()
{
	primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	_id = _idCount++;

	_name = "GameObject";
	_psoName = PSO_OPAQUE_SOLID;
	_tag = "Default";

	_numFramesDirty = RENDER->GetNumFrameResources();
	_isInitialized = false;
}

GameObject::~GameObject()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - GameObject:" << _id << "\n";
#endif

	if (!_isDestroyed)
		OnDestroy();
}

void GameObject::Init()
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			c->isInitialized = true;
			c->Init();
		}
	}
}

void GameObject::PreUpdate()
{
	if (!_isActive || _parentInactiveStack > 0) return;

	if (!_isInitialized) {
		_isInitialized = true;
		Init();
	}

	if (_isDeleteReserved) {
		_deleteTime -= TIME->DeltaTime();
		if (_deleteTime <= 0.0f) {
			auto& childs = GetTransform()->GetChilds();
			RENDER->DeleteGameobject(shared_from_this());
			return;
		}
	}

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (!c->IsActive()) continue;

			// 런타임 중 추가되는 컴포넌트
			if (!c->isInitialized) {
				c->isInitialized = true;
				c->Init();
			}

			if ((UINT32)c->type & ((UINT32)ComponentType::Rigidbody))
				continue;

			c->PreUpdate();
		}
	}
}

void GameObject::Update()
{
	if (!_isActive || _parentInactiveStack > 0) return;

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (!c->IsActive()) continue;

			if ((UINT32)c->type & ((UINT32)ComponentType::ParticleEmitter | (UINT32)ComponentType::Animator))
				continue;
			c->Update();
		}
	}
}

void GameObject::Render(ID3D12GraphicsCommandList* cmdList, UINT renderState)
{
	if (!_isActive || _parentInactiveStack > 0) return;

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (!c->IsActive()) continue;

			if ((UINT32)c->type & (UINT32)ComponentType::ParticleEmitter)
				continue;
			c->Render(cmdList, renderState);
		}
	}
}

void GameObject::OnCollisionEnter(shared_ptr<GameObject> other)
{
	if (!_isActive || _parentInactiveStack > 0) return;

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (!c->IsActive()) continue;

			c->OnCollisionEnter(other);
		}
	}
}

void GameObject::OnCollision(shared_ptr<GameObject> other)
{
	if (!_isActive || _parentInactiveStack > 0) return;

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (!c->IsActive()) continue;

			c->OnCollision(other);
		}
	}
}

void GameObject::OnCollisionExit(shared_ptr<GameObject> other)
{
	if (!_isActive || _parentInactiveStack > 0) return;

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (!c->IsActive()) continue;

			c->OnCollisionExit(other);
		}
	}
}

void GameObject::OnDestroy()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "OnDestroy - GameObject:" << _id << "\n";
#endif

	auto childs = GetTransform()->GetChilds();
	for (auto& c : childs) {
		c->OnDestroy();
	}

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (c == nullptr) continue;		// Already released by RenderManager

			c->OnDestroy();
			c.reset();
			--_componentCount;
		}

		componentVec.clear();
	}

	_isDestroyed = true;
}

shared_ptr<GameObject> GameObject::Instantiate()
{
	shared_ptr<GameObject> go = make_shared<GameObject>();
	RENDER->AddGameObject(go);
	return go;
}

shared_ptr<GameObject> GameObject::LoadPrefab(string filePath)
{
	// shared_ptr<GameObject> go = RESOURCE->LoadPrefab(filePath)[0];
	shared_ptr<GameObject> go = RESOURCE->LoadPrefabXML(filePath);

	if (go != nullptr) {
		go->_isPrefab = true;
		go->_prefabPath = filePath;
	}
	return go;
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	int componentTypeIdx = GetComponentTypeIndex(component->type);
	if (componentTypeIdx == -1) return;
	
	++_componentCount;
	component->SetGameObject(shared_from_this());
	_components[componentTypeIdx].push_back(component);
}

shared_ptr<Transform> GameObject::GetTransform()
{
	auto transform = GetComponent<Transform>();
	if (transform == nullptr) {
		transform = make_shared<Transform>();
		AddComponent(transform);
	}
	return transform;
}

int GameObject::GetComponentTypeIndex(ComponentType type)
{
	if (type == ComponentType::Undefined)			return -1;
	if (type == ComponentType::Transform)			return 0;
	if (type == ComponentType::MeshRenderer)		return 1;
	if (type == ComponentType::SkinnedMeshRenderer) return 2;
	if (type == ComponentType::Camera)				return 3;
	if (type == ComponentType::Script)				return 4;
	if (type == ComponentType::Light)				return 5;
	if (type == ComponentType::Animator)			return 6;
	if (type == ComponentType::Rigidbody)			return 7;
	if (type == ComponentType::ParticleEmitter)		return 8;
	if (type == ComponentType::CharacterController) return 9;
}

void GameObject::SetPSOName(const string& name)
{
	RENDER->UpdateObjectPSO(shared_from_this(), name);
	_psoName = name;
}

void GameObject::SetFramesDirty()
{
	_numFramesDirty = RENDER->GetNumFrameResources();
}

void GameObject::SetActive(bool flag)
{
	if (_isActive == flag) return;

	_isActive = flag;
	auto& childs = GetTransform()->GetChilds();
	if (_isActive) {
		for (auto& t : childs) {
			--t->GetGameObject()->_parentInactiveStack;
		}
	}
	else {
		for (auto& t : childs) {
			++t->GetGameObject()->_parentInactiveStack;
		}
	}
}

void GameObject::SetPrefabPath(string path)
{
	_prefabPath = path;
	_isPrefab = true;
}

void GameObject::Delete(float time)
{
	_isDeleteReserved = true;
	_deleteTime = time;
}

GameObjectSnapshot GameObject::CaptureSnapshot()
{
	_isSnapshotCaptured = true;

	GameObjectSnapshot snapshot;

	snapshot.id = _id;
	snapshot.name = _name;
	snapshot.pso = _psoName;

	return snapshot;
}

void GameObject::RestoreSnapshot(GameObjectSnapshot snapshot)
{
	_isSnapshotCaptured = false;
	SetName(snapshot.name);
	SetPSOName(snapshot.pso);
}
