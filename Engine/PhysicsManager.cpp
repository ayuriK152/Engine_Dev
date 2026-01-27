#include "pch.h"
#include "PhysicsManager.h"

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Init()
{
	// 1. 전역 시스템 초기화
	JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	// 2. 메모리 및 잡 시스템 생성
	_tempAlloc = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10MB
	_jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

	// 3. 인터페이스 할당
	_bpLayerInterface = new BPLayerInterfaceImpl();
	_objVsBPFilter = new ObjectVsBPFilterImpl();
	_objLayerFilter = new ObjectLayerPairFilterImpl();

	// 4. 물리 시스템 생성
	_physicsSystem = new JPH::PhysicsSystem();
	const UINT maxBodies = 1024;
	const UINT numBodyMutexes = 0;
	const UINT maxBodyPairs = 1024;
	const UINT maxContactConstraints = 1024;

	_physicsSystem->Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints,
		*_bpLayerInterface,
		(JPH::ObjectVsBroadPhaseLayerFilter&)*_objVsBPFilter,
		(JPH::ObjectLayerPairFilter&)*_objLayerFilter);
}

void PhysicsManager::FixedUpdate()
{

}

void PhysicsManager::Update()
{
	_physicsSystem->Update(TIME->DeltaTime(), 1, _tempAlloc, _jobSystem);

	for (auto& rigidbody : _rigidbodies) {
		if (!_physicsSystem->GetBodyInterface().IsActive(rigidbody->GetBodyID()))
			continue;

		JPH::RVec3 position;
		JPH::Quat rotation;
		_physicsSystem->GetBodyInterface().GetPositionAndRotation(rigidbody->GetBodyID(), position, rotation);

		// DX12 엔진의 Transform 업데이트
		Vector3 pos(position.GetX(), position.GetY(), position.GetZ());
		Vector4 rot(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());
		pos = pos - XMVector3Rotate(XMLoadFloat3(&rigidbody->GetColliderOffset()), XMLoadFloat4(&rot));
		rigidbody->GetTransform()->SetLocalPosition(pos);
		rigidbody->GetTransform()->SetQuaternion(rot);
	}
}

void PhysicsManager::LateUpdate()
{

}

void PhysicsManager::DeleteCollider(shared_ptr<Collider> collider)
{
	for (int i = 0; i < _colliders.size(); i++)
	{
		if (_colliders[i] == collider)
		{
			_colliders.erase(_colliders.begin() + i);
			break;
		}
	}
}

void PhysicsManager::DeleteRigidbody(shared_ptr<Rigidbody> rbd)
{
	for (int i = 0; i < _rigidbodies.size(); i++)
	{
		if (_rigidbodies[i] == rbd)
		{
			_rigidbodies.erase(_rigidbodies.begin() + i);
			break;
		}
	}
}
