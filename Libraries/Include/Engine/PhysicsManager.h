#pragma once

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
	void AddCollider(shared_ptr<Collider> collider) { _colliders.push_back(collider); }
	void DeleteCollider(shared_ptr<Collider> collider);

	void ResolvePenetration(CollisionInfo& collInfo, shared_ptr<Rigidbody>& rba, shared_ptr<Rigidbody>& rbb);

private:
	vector<shared_ptr<Collider>> _colliders;
};

