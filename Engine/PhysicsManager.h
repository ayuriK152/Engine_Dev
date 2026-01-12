#pragma once

// BroadPhase 레이어 정의
namespace Layers {
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

// 객체 간 충돌 여부 결정
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
		switch (inObject1) {
		case Layers::NON_MOVING: return inObject2 == Layers::MOVING; // 고정체는 이동체와만 충돌
		case Layers::MOVING: return true; // 이동체는 모두와 충돌
		default: return false;
		}
	}
};

// BroadPhase(공간 분할) 레이어 정의
namespace BroadPhaseLayers {
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr UINT NUM_LAYERS = 2;
}

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
	BPLayerInterfaceImpl() {
		_objectToBP[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		_objectToBP[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}
	virtual UINT GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }
	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { return _objectToBP[inLayer]; }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif
private:
	JPH::BroadPhaseLayer _objectToBP[Layers::NUM_LAYERS];
};

// 이건 뭘까
class ObjectVsBPFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

class PhysicsManager
{
	DECLARE_SINGLE(PhysicsManager);
public:
	~PhysicsManager();

	void Init();
	void FixedUpdate();
	void Update();
	void LateUpdate();

public:
	JPH::PhysicsSystem* GetPhysicsSystem() { return _physicsSystem; }

	// Legacy, 삭제 고려
	void AddCollider(shared_ptr<Collider> collider) { _colliders.push_back(collider); }
	void DeleteCollider(shared_ptr<Collider> collider);

	void AddRigidbody(shared_ptr<Rigidbody> rbd) { _rigidbodies.push_back(rbd); }
	void DeleteRigidbody(shared_ptr<Rigidbody> rbd);

private:
	vector<shared_ptr<Collider>> _colliders;
	vector<shared_ptr<Rigidbody>> _rigidbodies;

	JPH::PhysicsSystem* _physicsSystem = nullptr;
	JPH::JobSystem* _jobSystem = nullptr;
	JPH::TempAllocator* _tempAlloc = nullptr;

	BPLayerInterfaceImpl* _bpLayerInterface = nullptr;
	ObjectVsBPFilterImpl* _objVsBPFilter = nullptr;
	ObjectLayerPairFilterImpl* _objLayerFilter = nullptr;
};

