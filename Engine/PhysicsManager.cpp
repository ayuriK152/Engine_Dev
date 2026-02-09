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

	_physicsSystem->SetContactListener(this);
}

void PhysicsManager::PreUpdate()
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

void PhysicsManager::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	auto obj1 = reinterpret_cast<GameObject*>(inBody1.GetUserData())->shared_from_this();
	auto obj2 = reinterpret_cast<GameObject*>(inBody2.GetUserData())->shared_from_this();

	obj1->OnCollisionEnter(obj2);
	obj2->OnCollisionEnter(obj1);
}

void PhysicsManager::OnContactAdded(const CharacterVirtual* inCharacter, const BodyID& inBodyID2, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
	JPH::BodyLockRead lock(_physicsSystem->GetBodyLockInterface(), inBodyID2);

	if (lock.Succeeded()) {
		auto obj1 = reinterpret_cast<GameObject*>(inCharacter->GetUserData())->shared_from_this();
		auto obj2 = reinterpret_cast<GameObject*>(lock.GetBody().GetUserData())->shared_from_this();

		obj1->OnCollisionEnter(obj2);
		obj2->OnCollisionEnter(obj1);
	}
}

void PhysicsManager::OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void PhysicsManager::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	auto obj1 = reinterpret_cast<GameObject*>(inBody1.GetUserData())->shared_from_this();
	auto obj2 = reinterpret_cast<GameObject*>(inBody2.GetUserData())->shared_from_this();

	obj1->OnCollision(obj2);
	obj2->OnCollision(obj1);
}

void PhysicsManager::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{

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

vector<shared_ptr<GameObject>> PhysicsManager::OverlapSphere(Vector3 position, float radius, string tag)
{
	vector<shared_ptr<GameObject>> results;

	JPH::SphereShape sphere(radius);
	JPH::Vec3 spherePos(position.x, position.y, position.z);

	// 필요한 경우 세팅부분 추가
	JPH::CollideShapeSettings settings;

	JPH::AllHitCollisionCollector<JPH::CollideShapeCollector> collector;

	_physicsSystem->GetNarrowPhaseQuery().CollideShape(
		&sphere,
		JPH::Vec3::sReplicate(1.0f),
		JPH::Mat44::sTranslation(spherePos),
		settings,
		spherePos,
		collector
	);

	for (const JPH::CollideShapeResult& hit : collector.mHits) {

		JPH::BodyLockRead lock(_physicsSystem->GetBodyLockInterface(), hit.mBodyID2);
		
		if (lock.Succeeded()) {
			// if (lock.GetBody().IsSensor()) continue;

			shared_ptr<GameObject> go = reinterpret_cast<GameObject*>(lock.GetBody().GetUserData())->shared_from_this();
			if (go->GetTag() == tag || tag == "") {
				results.push_back(go);
			}
		}
	}

	DEBUG->Log(to_string(results.size()));
	return results;
}
