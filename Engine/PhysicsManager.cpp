#include "pch.h"
#include "PhysicsManager.h"

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Init()
{// 1. 전역 시스템 초기화
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
	/*
	if (_colliders.size() == 0)
		return;

	for (int i = 0; i < _colliders.size(); i++) {
		if (_colliders[i]->GetGameObject() == nullptr) {
			_colliders.erase(_colliders.begin() + i);
			i--;
			continue;
		}

		_colliders[i]->_isOnColliding = false;
		_colliders[i]->Update();
	}

	for (int i = 0; i < _colliders.size() - 1; i++)
	{
		if (!_colliders[i]->IsActive())
			continue;

		for (int j = i + 1; j < _colliders.size(); j++)
		{
			if (!_colliders[j]->IsActive())
				continue;

			Vector3 normal;
			float depth;
			CollisionInfo collInfo = _colliders[i]->CheckCollide(_colliders[j]);

			if (collInfo.IsCollide)
			{
				_colliders[i]->_isOnColliding = true;
				_colliders[j]->_isOnColliding = true;

				_colliders[i]->GetGameObject()->OnCollision(_colliders[j]);
				_colliders[j]->GetGameObject()->OnCollision(_colliders[i]);

				if (!_colliders[i]->_isTrigger && !_colliders[j]->_isTrigger) {
					shared_ptr<Rigidbody> rba = _colliders[i]->GetGameObject()->GetComponent<Rigidbody>();
					shared_ptr<Rigidbody> rbb = _colliders[j]->GetGameObject()->GetComponent<Rigidbody>();

					if (rba != nullptr || rbb != nullptr)
						ResolvePenetration(collInfo, rba, rbb);
				}
			}
		}
	}
	*/
	_physicsSystem->Update(TIME->DeltaTime(), 1, _tempAlloc, _jobSystem);

	for (auto& rigidbody : _rigidbodies) {
		JPH::BodyInterface& bodyInterface = _physicsSystem->GetBodyInterface();

		JPH::RVec3 position;
		JPH::Quat rotation;
		bodyInterface.GetPositionAndRotation(rigidbody->GetBodyID(), position, rotation);

		// DX12 엔진의 Transform 업데이트
		rigidbody->GetTransform()->SetLocalPosition(Vector3(position.GetX(), position.GetY(), position.GetZ()));
		rigidbody->GetTransform()->SetQuaternion(Vector4(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW()));
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

void PhysicsManager::ResolvePenetration(CollisionInfo& collInfo, shared_ptr<Rigidbody>& rba, shared_ptr<Rigidbody>& rbb)
{
	// 회전을 구현해두긴 했는데 조금 이상함. 개선 작업을 추후에 진행할 예정. 그 전까지 건드리지 않기.

	float massA = rba != nullptr ? rba->mass : 0.0f;
	float massB = rbb != nullptr ? rbb->mass : 0.0f;

	float resolveValue = massA == 0.0f || massB == 0.0f ? 1.0f : 0.5f;

	Vector3 correctionA, correctionB;

	if (rba == nullptr && rbb != nullptr)
	{
		if (rbb->isPenetrationNormalFixed && fabs(collInfo.Normal.y) > 1e-4)
		{
			collInfo.Depth = collInfo.Depth / sqrt(fabs(collInfo.Normal.y));
			collInfo.Normal = Vector3(0.0f, collInfo.Normal.y > 0 ? 1 : -1, 0.0f);
			correctionB = { 0.0f, collInfo.Depth, 0.0f };
		}
		correctionB = collInfo.Normal * collInfo.Depth;
	}
	else if (rba != nullptr && rbb == nullptr)
	{
		if (rba->isPenetrationNormalFixed && fabs(collInfo.Normal.y) > 1e-4)
		{
			collInfo.Depth = collInfo.Depth / sqrt(fabs(collInfo.Normal.y));
			collInfo.Normal = Vector3(0.0f, collInfo.Normal.y > 0 ? 1 : -1, 0.0f);
			correctionA = { 0.0f, collInfo.Depth, 0.0f };

		}
		correctionA = collInfo.Normal * collInfo.Depth;
	}
	else
	{
		correctionA = collInfo.Normal * collInfo.Depth * resolveValue;
		correctionB = collInfo.Normal * collInfo.Depth * resolveValue;
	}

	// 회전 운동 임펄스
	Vector3 impulse = collInfo.Normal * collInfo.Depth;

	if (rba != nullptr)
	{
		Vector3 pos = rba->GetTransform()->GetPosition();
		Vector3 vel = (rba->GetVelocity() * rba->elasticModulus).Reflect(collInfo.Normal);
		rba->SetVelocity(vel);
		Vector3 finalPos = pos - correctionA + vel * collInfo.Depth * resolveValue;

		rba->GetTransform()->SetPosition(finalPos);

		float velocityPower = XMVector3Length(XMLoadFloat3(&vel)).m128_f32[0];
		Vector3 r = collInfo.ContactPoint - rba->GetTransform()->GetPosition();
		Vector3 torque;
		XMStoreFloat3(&torque, XMVector3Cross(XMLoadFloat3(&impulse), XMLoadFloat3(&r)) * velocityPower);
		rba->AddTorque(torque);
	}

	if (rbb != nullptr)
	{
		Vector3 pos = rbb->GetTransform()->GetPosition();
		Vector3 vel = (rbb->GetVelocity() * rbb->elasticModulus).Reflect(collInfo.Normal);
		rbb->SetVelocity(vel);
		Vector3 finalPos = pos + correctionB + vel * collInfo.Depth * resolveValue;

		rbb->GetTransform()->SetPosition(finalPos);

		float velocityPower = XMVector3Length(XMLoadFloat3(&vel)).m128_f32[0];
		Vector3 r = collInfo.ContactPoint - rbb->GetTransform()->GetPosition();
		Vector3 torque;
		XMStoreFloat3(&torque, XMVector3Cross(XMLoadFloat3(&-impulse), XMLoadFloat3(&r)) * velocityPower);
		rbb->AddTorque(torque);
	}
}
